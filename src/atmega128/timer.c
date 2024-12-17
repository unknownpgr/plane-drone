#include "core/timer.h"
#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>

/**
 * 원래는 적당한 microsecond를 얻기 위하여 Timer2에서 적절한 분주비를 사용하려고 했다.
 * 그런데 알 수 없는 이유로 분주비가 예상한 것과 다르게 동작하는 현상을 발견했다.
 * 이는 무척 수상하므로 일단 분주비를 1로 설정한 후 이를 적절히 이용하여 microsecond를 계산하도록 했다.
 * timer2_overflow_count는 이에 따라 16MHz/256 = 62500Hz로 동작하며 16bit 타이머이므로 약 1.04초까지 측정할 수 있다.
 */

volatile uint16_t timer2_overflow_count = 0;

// Timer2 초기화 함수 (8-bit)
void timer_init()
{
  TCCR2 = (1 << CS20);       // 분주비 1로 설정 (16MHz)
  TCNT2 = 0;                 // 타이머/카운터 레지스터 초기화
  TIMSK |= (1 << TOIE2);     // 오버플로 인터럽트 활성화
  timer2_overflow_count = 0; // 오버플로 카운터 초기화
  sei();                     // 전역 인터럽트 활성화
}

// Timer2 오버플로 인터럽트 핸들러
ISR(TIMER2_OVF_vect)
{
  timer2_overflow_count++; // 16MHz/256 = 62500Hz
}

// 마이크로초 단위 타이머 함수
uint16_t timer_micros()
{
  /**
   * timer2_overflow_count는 16MHz/256 = 1/16MHz로 설정되어있으므로 왼쪽으로 4비트 시프트하면(x16) 단위가 us가 된다.
   * TCNT2는 반대로 16MHz로 설정되어있으므로 오른쪽으로 4비트 시프트하면(/16) 단위가 us가 된다.
   * 이 함수는 2^16 us = 65536 us = 65.536 ms까지만 측정할 수 있다.
   */
  return timer2_overflow_count << 4 | TCNT2 >> 4;
}

// 지정된 밀리초(ms) 동안 대기하는 함수
void timer_sleep(uint16_t ms)
{
  uint32_t current_tick = TCNT2;
  uint32_t end_tick = current_tick + (uint32_t)ms * 16000;

  while (current_tick < end_tick)
  {
    current_tick += (uint8_t)(TCNT2 - current_tick);
  }
}
