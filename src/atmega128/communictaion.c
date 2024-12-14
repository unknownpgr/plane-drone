#include "core/communication.h"
#include "core/uart.h"
#include "core/spi.h"
#include "core/util.h"
#include "nrf24l01.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

void communication_init()
{
    // Initialize Serial
    uart_init(9600);
    // Initialize NRF24L01
    nrf24_init();
}

// Serial Communication

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

// Radio Communication

void communication_send_radio(const char *format, ...)
{
}

bool communication_receive_radio(ControllerData *protocol)
{
    ControllerDataUnion *data = (ControllerDataUnion *)protocol;
    return !!nrf24_receive((uint8_t *)data->bytes, sizeof(ControllerData));
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