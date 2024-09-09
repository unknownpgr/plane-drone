# 고정익기형 드론 설계 및 제작

고정익기형 드론을 설계하고 제작한다. 이 프로젝트는 다음과 같은 목표를 가진다.

- 최대한 저렴하고 쉽게 구할 수 있는 재료 및 부품으로 설계한다.
- 드론의 기능에 있어 다음과 같은 순차적인 목표를 구성할 수 있도록 설계한다.
  1. 드론의 단순 활공 - 공기 역학적으로 안정적인 드론을 구성하여 단순히 던지기만 해도 어느정도 비행하는 기체를 구성한다.
  2. Flap을 사용한 roll 제어 - 전원을 공급하고 MCU에 기반한 flap 제어를 통해 roll 안정성을 확보한다.
  3. Flap을 사용한 pitch 제어 - 무동력으로 드론이 급상승하거나 급하강하지 않도록 pitch 제어를 구현한다.
  4. Thrust 제어 - 프로펠러를 사용하여 추력을 생성, 드론이 떨어지지 않도록 안정적인 비행을 구현한다.
  5. Control fallback 구성 - 드론이 비행 중 controller와의 연결이 끊어지더라도 안전하게 착륙할 수 있도록 heartbeat를 설계하고 timeout 시 throttle을 0으로 설정한다.
  6. 방향 제어 - 드론이 원하는 방향으로 비행하도록 구성한다.
  7. 자동착륙 - 드론이 땅에 안전하게 착륙할 수 있도록 구성한다.
  8. 자동비행 - 드론이 사용자의 입력 없이 정해진 루트를 따라 자율적으로 비행할 수 있도록 구성한다.
- 일반적인 펌웨어 구성과 다르게 클린 아키텍쳐에 기반하여 localizatoin 및 control 로직을 하드웨어와 분리한다.
  - 현재 `C++`보다 `C`에 익숙한 상태이므로 먼저 C의 헤더 파일을 통하여 이를 구현하고 추후 `C++`의 `virtual` 키워드를 사용하여 코어 계층을 구성할 수 있도록 한다.

## 소스 구조

- `plane/plane.ino`: Arduino Sketch를 사용하여 손쉽게 MCU에 소스코드를 업로드할 수 있도록 만든 진입점
- `plane/core`: 하드웨어 추상화 계층 (헤더 파일 모음)
- `plane/main.cpp`: 실제 소스코드 진입점 및 주 비즈니스 로직
- 그 외 다른 파일들: 하드웨어 추상화 계층의 구현체
