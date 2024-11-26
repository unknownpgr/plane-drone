#include "core/timer.h"
#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>

volatile uint16_t timer2_overflow_count = 0;

// Timer2 초기화 함수 (8-bit)
void timer_init()
{
  TCCR2 = (1 << CS20);   // 분주비 1로 설정
  TCNT2 = 0;             // 타이머/카운터 레지스터 초기화
  TIMSK |= (1 << TOIE2); // 오버플로 인터럽트 활성화
  sei();                 // 전역 인터럽트 활성화
}

// Timer2 오버플로 인터럽트 핸들러
ISR(TIMER2_OVF_vect)
{
  timer2_overflow_count++;
}

// 마이크로초 단위 타이머 함수
uint16_t timer_micros()
{
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
