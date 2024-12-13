#include "core/spi.h"
#include "nrf24l01.h"
#include "core/util.h"
#include <stdlib.h>

/* nRF24L01 Command */
#define RD_REG 0x00
#define WR_REG 0x20
#define RX_PAYLOAD_WD 0x60
#define RD_RX_PAYLOAD 0x61
#define WR_TX_PAYLOAD 0xA0
#define FLUSH_TX 0xE1
#define FLUSH_RX 0xE2
#define REUSE_TX_PL 0xE3
#define RD_RX_PL_WID 0x60
#define WR_ACK_PAYLOAD 0xA8
#define WR_TX_PL_NO_ACK 0xB0
#define NOP 0xFF

/* nRF24L01 Register Address */
#define CONFIG_REG 0x00
#define EN_AA_REG 0x01
#define EN_RXADDR_REG 0x02
#define SETUP_AW_REG 0x03
#define SETUP_RETR_REG 0x04
#define RF_CH_REG 0x05
#define RF_SETUP_REG 0x06
#define STATUS_REG 0x07
#define OBSERVE_TX_REG 0x08
#define RPD_REG 0x09
#define RX_ADDR_P0_REG 0x0A
#define RX_ADDR_P1_REG 0x0B
#define RX_ADDR_P2_REG 0x0C
#define RX_ADDR_P3_REG 0x0D
#define RX_ADDR_P4_REG 0x0E
#define RX_ADDR_P5_REG 0x0F
#define TX_ADDR_REG 0x10
#define RX_PWD_P0_REG 0x11
#define RX_PWD_P1_REG 0x12
#define RX_PWD_P2_REG 0x13
#define RX_PWD_P3_REG 0x14
#define RX_PWD_P4_REG 0x15
#define RX_PWD_P5_REG 0x16
#define FIFO_STATUS_REG 0x17
#define DYNPD_REG 0x1C
#define FEATURE_REG 0x1D

#define ADDR_WIDTH 5
#define PAYLOAD_WIDTH 32

/* nRF24L01 Config Register bit value */
#define MASK_RX_DR 0x40
#define MASK_TX_DS 0x20
#define MASK_MAX_RT 0x10
#define EN_CRC 0x08
#define CRC_2BYTES 0x04
#define POWER_UP 0x02
#define PRIM_RX 0x01

/* nRF24L01 Status Register bit value */
#define RX_DR 0x40
#define TX_DS 0x20
#define MAX_RT 0x10
#define TX_FULL 0x01

enum nrf_pipe
{
    NRF_PIPE0,     /**< Select pipe0 */
    NRF_PIPE1,     /**< Select pipe1 */
    NRF_PIPE2,     /**< Select pipe2 */
    NRF_PIPE3,     /**< Select pipe3 */
    NRF_PIPE4,     /**< Select pipe4 */
    NRF_PIPE5,     /**< Select pipe5 */
    NRF_TX,        /**< Refer to TX address*/
    NRF_ALL = 0xFF /**< Close or open all pipes*/
};

enum crc_mode
{
    NRF_CRC_OFF,      /**< CRC check disabled */
    NRF_CRC_8BIT = 2, /**< CRC check set to 8-bit */
    NRF_CRC_16BIT     /**< CRC check set to 16-bit */
};

enum address_width
{
    NRF_AW_3BYTES = 1, /**< Set address width to 3 bytes */
    NRF_AW_4BYTES,     /**< Set address width to 4 bytes */
    NRF_AW_5BYTES      /**< Set address width to 5 bytes */
};

enum op_mode
{
    NRF_PTX, /**< Primary TX operation */
    NRF_PRX  /**< Primary RX operation */
};

enum pwr_mode
{
    NRF_PWR_DOWN, /**< Device power-down */
    NRF_PWR_UP    /**< Device power-up */
};

enum data_rate
{
    NRF_1MBPS, /**< Datarate set to 1 Mbps  */
    NRF_2MBPS  /**< Datarate set to 2 Mbps  */
};

enum output_pwr
{
    NRF_18DBM, /**< Output power set to -18dBm */
    NRF_12DBM, /**< Output power set to -12dBm */
    NRF_6DBM,  /**< Output power set to -6dBm  */
    NRF_0DBM   /**< Output power set to 0dBm   */
};

/*============================================================================*/

static uint8_t nrf24_read_reg(uint8_t reg, uint8_t *data, int len)
{
    spi_csn(LOW);
    spi_transfer(reg);
    if (data && len)
    {
        for (int i = 0; i < len; i++)
        {
            *data++ = spi_transfer(NOP);
        }
    }
    uint8_t val = spi_transfer(NOP);
    spi_csn(HIGH);
    return val;
}

static void nrf24_read_payload(uint8_t *data, int len, uint8_t width)
{
    uint8_t dummy = width - len;

    spi_csn(LOW);
    spi_transfer(RD_RX_PAYLOAD);
    while (len--)
        *data++ = spi_transfer(NOP);
    while (dummy--)
        spi_transfer(NOP);
    spi_csn(HIGH);
}

static uint8_t nrf24_write_reg(uint8_t reg, uint8_t *data, int len)
{
    spi_csn(LOW);
    uint8_t val = spi_transfer(WR_REG | reg);
    if (data && len)
    {
        for (int i = 0; i < len; i++)
        {
            spi_transfer(data[i]);
        }
    }
    spi_csn(HIGH);
    return val;
}

static void nrf24_write_payload(uint8_t *data, int len)
{
    uint8_t dummy = PAYLOAD_WIDTH - len;

    spi_csn(LOW);
    spi_transfer(WR_TX_PAYLOAD);
    while (len--)
        spi_transfer(*data++);
    while (dummy--)
        spi_transfer(0);
    spi_csn(HIGH);
}

static void nrf24_close_pipe(enum nrf_pipe pipe)
{
    uint8_t aa = 0;
    uint8_t rxaddr = 0;

    if (pipe != NRF_ALL)
    {
        aa = nrf24_read_reg(EN_AA_REG, NULL, 0) & ~(1 << pipe);
        rxaddr = nrf24_read_reg(EN_RXADDR_REG, NULL, 0) & ~(1 << pipe);
    }

    nrf24_write_reg(EN_AA_REG, &aa, 1);
    nrf24_write_reg(EN_RXADDR_REG, &rxaddr, 1);
}

static void nrf24_open_pipe(enum nrf_pipe pipe)
{
    uint8_t aa = 0x3F;
    uint8_t rxaddr = 0x3F;

    if (pipe != NRF_ALL)
    {
        aa = nrf24_read_reg(EN_AA_REG, NULL, 0) | (1 << pipe);
        rxaddr = nrf24_read_reg(EN_RXADDR_REG, NULL, 0) | (1 << pipe);
    }

    nrf24_write_reg(EN_AA_REG, &aa, 1);
    nrf24_write_reg(EN_RXADDR_REG, &rxaddr, 1);
}

static void nrf24_crc_mode(enum crc_mode crc)
{
    uint8_t config = 0;

    config = (nrf24_read_reg(CONFIG_REG, NULL, 0) & ~0x0C) | (crc << 2);
    nrf24_write_reg(CONFIG_REG, &config, 1);
}

static void nrf24_auto_retr(uint8_t arc, uint16_t ard)
{
    uint8_t data = (((ard / 250) - 1) << 4) | arc;

    nrf24_write_reg(SETUP_RETR_REG, &data, 1);
}

static void nrf24_addr_width(enum address_width aw)
{
    nrf24_write_reg(SETUP_AW_REG, (uint8_t *)&aw, 1);
}

static void nrf24_set_addr(enum nrf_pipe pipe, uint8_t *addr)
{
    uint8_t aw = nrf24_read_reg(SETUP_AW_REG, NULL, 0) + 2;

    switch (pipe)
    {
    case NRF_PIPE0:
    case NRF_PIPE1:
    case NRF_TX:
        nrf24_write_reg(RX_ADDR_P0_REG + pipe, addr, aw);
        break;

    case NRF_PIPE2 ... NRF_PIPE5:
        nrf24_write_reg(RX_ADDR_P0_REG + pipe, addr, 1);
        break;

    default:
        break;
    }
}

static void nrf24_op_mode(enum op_mode mode)
{
    uint8_t config = nrf24_read_reg(CONFIG_REG, NULL, 0);

    if (mode == NRF_PTX)
    {
        config &= ~PRIM_RX;
    }
    else
    {
        config |= PRIM_RX;
    }

    nrf24_write_reg(CONFIG_REG, &config, 1);
}

static void nrf24_payload_width(enum nrf_pipe pipe, uint8_t width)
{
    nrf24_write_reg(RX_PWD_P0_REG + pipe, &width, 1);
}

static void nrf24_rf_channel(uint8_t rf_ch)
{
    nrf24_write_reg(RF_CH_REG, (uint8_t *)&rf_ch, 1);
}

static void nrf24_rf_data_rate(enum data_rate bps)
{
    uint8_t rf = nrf24_read_reg(RF_SETUP_REG, NULL, 0);

    if (bps == NRF_1MBPS)
    {
        rf &= ~(1 << 3);
    }
    else
    {
        rf |= (1 << 3);
    }

    nrf24_write_reg(RF_SETUP_REG, &rf, 1);
}

static void nrf24_power_mode(enum pwr_mode mode)
{
    uint8_t config = nrf24_read_reg(CONFIG_REG, NULL, 0);

    if (mode == NRF_PWR_DOWN)
    {
        config &= ~POWER_UP;
    }
    else
    {
        config |= POWER_UP;
    }

    nrf24_write_reg(CONFIG_REG, &config, 1);
}

static void nrf24_flush_tx(void)
{
    nrf24_write_reg(FLUSH_TX, NULL, 0);
}

static void nrf24_flush_rx(void)
{
    nrf24_write_reg(FLUSH_RX, NULL, 0);
}

/*============================================================================*/

static uint8_t tranceiver_addr[ADDR_WIDTH] = "00001";

void nrf24_init(void)
{
    spi_init();

    spi_ce(LOW);
    spi_csn(HIGH);
    util_delay_ms(10);

    nrf24_close_pipe(NRF_ALL);
    nrf24_open_pipe(NRF_PIPE0);
    nrf24_crc_mode(NRF_CRC_16BIT);
    nrf24_auto_retr(15, 500);

    nrf24_addr_width(NRF_AW_5BYTES);
    nrf24_set_addr(NRF_TX, tranceiver_addr);
    nrf24_set_addr(NRF_PIPE0, tranceiver_addr);

    nrf24_op_mode(NRF_PRX);
    nrf24_payload_width(NRF_PIPE0, PAYLOAD_WIDTH);
    nrf24_rf_channel(76);
    nrf24_rf_data_rate(NRF_1MBPS);

    nrf24_flush_tx();
    nrf24_flush_rx();

    nrf24_power_mode(NRF_PWR_UP);

    util_delay_ms(10);

    spi_ce(HIGH);
}

int nrf24_receive(uint8_t *buf, uint8_t len)
{
    uint8_t status = nrf24_read_reg(STATUS_REG, NULL, 0);
    if (status & RX_DR)
    {
        // uint8_t pipe = (nrf24_read_reg(STATUS_REG, NULL, 0) >> 1) & 7;
        uint8_t width = nrf24_read_reg(RX_PAYLOAD_WD, NULL, 0);
        nrf24_read_payload(buf, len, width);
        uint8_t clear = RX_DR;
        nrf24_write_reg(STATUS_REG, &clear, 1);
        return width;
    }
    return 0;
}
