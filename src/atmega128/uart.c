#include <avr/io.h>
#include "core/uart.h"

#define F_CPU 16000000UL

void uart_init(uint32_t baudrate)
{
    // Baud rate 설정
    unsigned int ubrr = F_CPU / 16 / baudrate - 1;
    UBRR0H = (unsigned char)(ubrr >> 8); // 상위 바이트
    UBRR0L = (unsigned char)ubrr;        // 하위 바이트

    // UART 송수신 활성화
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);

    // 비동기, 8비트 데이터, 1비트 스톱 비트
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void uart_write(uint8_t *data, uint32_t len)
{
    for (uint32_t i = 0; i < len; i++)
    {
        // 전송 가능 대기
        while (!(UCSR0A & (1 << UDRE0)))
            ;
        UDR0 = data[i];
    }
}

void uart_read(uint8_t *data, uint32_t len)
{
    for (uint32_t i = 0; i < len; i++)
    {
        // 수신 가능 대기
        while (!(UCSR0A & (1 << RXC0)))
            ;
        data[i] = UDR0;
    }
}