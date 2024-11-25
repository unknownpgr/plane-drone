#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU 16000000UL // ATmega128의 클럭 주파수 16MHz 기준
#define TIMER2_PRESCALER 64
#define TIMER2_TICK_MICROSECONDS (TIMER2_PRESCALER * 1000000UL / F_CPU)

static volatile uint32_t overflow_count = 0;

// Timer2 초기화 함수
void timer_init()
{
  TCCR2 = (1 << CS22); // 분주비 64 설정 (16MHz -> 250kHz -> 4μs)
  TCNT2 = 0;           // 타이머 카운터 초기화

  TIMSK |= (1 << TOIE2); // Timer2 오버플로 인터럽트 활성화
  sei();                 // 전역 인터럽트 활성화
}

// Timer2 오버플로 인터럽트 서비스 루틴
ISR(TIMER2_OVF_vect)
{
  overflow_count++;
}

// 마이크로초 단위 타이머 함수
uint16_t timer_micros()
{
  uint32_t count;
  uint8_t timer_count;

  cli(); // 인터럽트 비활성화
  count = overflow_count;
  timer_count = TCNT2;
  sei(); // 인터럽트 활성화

  // 오버플로 값(256) 및 틱당 마이크로초를 이용해 경과 시간 계산
  return (count * 256 + timer_count) * TIMER2_TICK_MICROSECONDS;
}

// 지정된 밀리초(ms) 동안 대기하는 함수
void timer_sleep(uint16_t ms)
{
  uint16_t start_time = timer_micros();
  while ((timer_micros() - start_time) < (ms * 1000))
  {
    // Busy wait
  }
}
