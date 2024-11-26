#include "core/communication.h"
#include "core/uart.h"
#include "core/spi.h"
#include "core/util.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#define RF24_W_REGISTER 0x20

#define RF24_PRIM_RX (((uint8_t)1 << 0))
#define RF24_PWR_UP (((uint8_t)1) << 1)
#define RF24_CRC0 (((uint8_t)1) << 2)
#define RF24_EN_CRC (((uint8_t)1) << 3)
#define RF24_MAX_RT (((uint8_t)1) << 4)
#define RF24_TX_DS (((uint8_t)1) << 5)
#define RF24_RX_DR (((uint8_t)1) << 6)

static void rf24_write_register(uint8_t reg, uint8_t value)
{
    spi_csn(LOW);
    spi_transfer(RF24_W_REGISTER | reg);
    spi_transfer(value);
    spi_csn(HIGH);
}

static void rf24_write_register_multi(uint8_t reg, uint8_t *data, uint8_t length)
{
    spi_csn(LOW);
    spi_transfer(RF24_W_REGISTER | reg);
    for (uint8_t i = 0; i < length; i++)
    {
        spi_transfer(data[i]);
    }
    spi_csn(HIGH);
}

static uint8_t rf24_read_register(uint8_t reg)
{
    spi_csn(LOW);
    spi_transfer(reg);
    uint8_t value = spi_transfer(0xFF);
    spi_csn(HIGH);
    return value;
}

static void rf24_read_payload(uint8_t *data, uint8_t length)
{
    spi_csn(LOW);
    spi_transfer(0x61); // R_RX_PAYLOAD
    for (uint8_t i = 0; i < length; i++)
    {
        data[i] = spi_transfer(0xFF); // Read payload
    }
    spi_csn(HIGH);
}

static void rf24_init()
{
    spi_init();

    // Delay for 10ms for the module to power up
    util_delay_ms(10);

    spi_ce(LOW);

    // Check if the module is connected
    rf24_write_register(0x00, 0x0F);
    uint8_t config = rf24_read_register(0x00);
    uint8_t status = rf24_read_register(0x07);
    communication_send_serial("CONFIG: 0x%02X\n", config);
    communication_send_serial("STATUS: 0x%02X\n", status);

    // Initialize NRF24L01
    rf24_write_register(0x01, 0x3F); // EN_AA, Enable Auto Ack: 0x3F
    rf24_write_register(0x02, 0x03); // EN_RXADDR, Enable Data Pipe 0, 1
    rf24_write_register(0x03, 0x03); // SETUP_AW, Address Width: 5 bytes
    rf24_write_register(0x04, 0x04); // SETUP_RETR, Retransmit Delay: 4000us, Retransmit Count: 15
    rf24_write_register(0x05, 76);   // RF_CH, Channel: 76
    rf24_write_register(0x06, 0x07); // RF_SETUP, RF Data Rate: 1Mbps, RF Output Power: 0dBm

    // Clear status
    rf24_write_register(0x07, RF24_RX_DR | RF24_TX_DS | RF24_MAX_RT);

    // Flush RX FIFO / TX FIFO
    rf24_read_register(0xE2); // FLUSH_RX
    rf24_read_register(0xE1); // FLUSH_TX

    // Set RX Mode
    rf24_write_register(0x00, RF24_EN_CRC | RF24_CRC0);
    rf24_write_register(0x00, RF24_EN_CRC | RF24_CRC0 | RF24_PWR_UP | RF24_PRIM_RX);
    util_delay_ms(10);

    // Check if the configuration is correct
    config = rf24_read_register(0x00);
    communication_send_serial("CONFIG CHECK: ");
    if (config == RF24_EN_CRC | RF24_CRC0 | RF24_PWR_UP | RF24_PRIM_RX)
    {
        communication_send_serial("OK\n");
    }
    else
    {
        communication_send_serial("FAIL\n");
    }

    // Set RX Address
    uint8_t rx_addr[5] = "00001";
    rf24_write_register_multi(0x0A, rx_addr, 5);

    spi_ce(HIGH);
}

static void rf24_test_receive()
{
    uint8_t status;
    uint8_t data[32]; // 수신 데이터 저장용

    while (1)
    {
        status = rf24_read_register(0x17);
        if (status & 1)
            continue;

        rf24_read_payload(data, 32);
        rf24_write_register(0x07, (1 << 6));
        communication_send_serial("Received: ");
        for (int i = 0; i < 32; i++)
        {
            communication_send_serial("%x", data[i]);
        }
        communication_send_serial("\n");
    }
}

void communication_init()
{
    // Initialize Serial
    uart_init(9600);

    // Initialize NRF20L01
    rf24_init();

    communication_send_serial("INITIALIZED\n");
}

void communication_send_serial(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    char buffer[256];
    vsnprintf(buffer, 256, format, args);
    va_end(args);
    uart_write((uint8_t *)buffer, (uint32_t)strlen(buffer));
}

void communication_receive_serial(char *buffer, uint32_t size)
{
    uart_read((uint8_t *)buffer, size);
}

void communication_send_radio(const char *format, ...)
{
}

bool communication_receive_radio(RadioProtocol *protocol)
{
    // if (radio.available())
    // {
    //     radio.read(protocol, sizeof(RadioProtocol));
    //     return true;
    // }
    return false;
}

void communication_send_ack(uint8_t *bytes, uint8_t size)
{
    // uint8_t ack[10] = {0};
    // for (int i = 0; i < size; i++)
    // {
    //     ack[i] = bytes[i];
    // }
    // radio.writeAckPayload(0, bytes, size); // ACK 패킷에 데이터 포함
}