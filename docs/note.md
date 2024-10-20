# Note

## 하드웨어 구성

- MPU6050이나 NRF24L01처럼 핀이 많고 복잡한 모듈을 연결하는 방법이 고민이다.
  - 가장 단순한 방법은 PCB를 만들어 고정하는 것이다. 그러나 PCB는 여러 단점이 있다.
    - 일단 비용이 비싸고 제작하는데 시간이 오래 걸린다. 이는 이 비행기의 최대한 단순하게 만든다는 철학과는 잘 맞지 않는다.
    - 특히 부품의 변화에 무척 취약하다. 부품 자체를 다른 종류로 바꾸는 것은 아예 불가능하고 한번 납땜하면 부품 고장 시 떼기도 어렵다.
    - 연결 강도 역시 의문스럽다. 큰 부품이 핀을 몇 개만 가지는 경우, 이것을 어떻게 PCB에 안전하게 고정해야 하는가?
  - 가장 유연한 방법은 점퍼 케이블을 사용하는 것이다.
    - 그러나 점퍼 케이블은 무엇보다 너무 잘 빠진다.
    - 점퍼 케이블을 핀헤더에 연결하거나, 끝부분을 접착제로 묶으면 아마도 잘 빠지지 않을 것이다.
    - 그러나 점퍼 케이블을 전부 하나로 묶어버리면 프로세서단에서 여러 부품의 점퍼 케이블이 결합되어야만 한다. 이것은 무척 좋지 않다.
  - 이것들을 잘 절충하면 다음과 같은 방식으로 구성할 수 있다.
    - 부품 쪽은 부품의 핀 배치에 맞춰서, 프로세서 쪽은 그냥 일렬로 배치한 점퍼 케이블을 만든다.
      - 점퍼 케이블의 끝단을 접착제로 고정하면 쉽게 만들 수 있을 것이다.
      - molex를 사용하는 것도 좋은 방법일 수 있다. 강도와 형상에 있어 이점이 크다.
      - 물론 그냥 핀헤더나 박스 헤더를 사용해도 충분할 것 같다.
    - 이러한 케이블과 프로세서를 연결하는 어댑터 보드를 만든다.
      - 이 어댑터 보드는 단순히 프로세서와 케이블의 연결 역할만 담당하므로 처음에는 만능 기판으로 만들고 이후 필요에 따라 PCB로 만들어도 된다.

## 회전에 대해 알아야 할 내용

- Rotation axis theorm: https://en.wikipedia.org/wiki/Euler%27s_rotation_theorem
- Rodrigues' rotation formula: https://en.wikipedia.org/wiki/Rodrigues%27_rotation_formula
- Infinitesimal rotation: https://en.wikipedia.org/wiki/Infinitesimal_rotation
- Gram-Schmidt process: https://en.wikipedia.org/wiki/Gram%E2%80%93Schmidt_process

- MPU6050은 우수좌표계 사용
