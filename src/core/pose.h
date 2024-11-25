#pragma once

/**
 * IMU 센서로부터 받은 각속도와 가속도를 기반으로 자세를 추정한다.
 * 각속도의 단위는 rad/s, 가속도의 단위는 m/s^2이다.
 *
 * roll, pitch, yaw 회전축은 순서대로 우수좌표계를 구성한다.
 * 즉, roll=x축은 전면, pitch=y축은 좌측, yaw=z축은 상단을 가리킨다.
 * 또한 좌표계의 일관성을 위하여 pitch는 일반적으로 설정하는 바와 다르게 기수가 아래로 갈수록 양수임에 주의하라.
 * 이는 회전 방향이 축에 대한 오른손 법칙을 일관적으로 따르도록 하기 위함이다.
 *
 * Acceleration의 각 값은 해당 축 방향으로의 가속도를 의미한다.
 * 즉, 센서와 로봇이 둘다 정위이고 정지한 경우 중력가속도에 의한 yaw=z축 가속도는 양수다.
 *
 * estimateByAngularVelocity와 estimateByComplementaryFilter 함수에서 state와 result가 분리된 것은 추후 Kalman Filter 등 다른 방법으로 추정할 때를 대비한 것이다.
 * 이 두 함수를 단순히 업데이트용으로 사용할 때는 같은 포인터를 사용해도 무방하다.
 */

typedef float Scalar;

typedef struct EulerAngle
{
  Scalar roll;
  Scalar pitch;
  Scalar yaw;
} EulerAngle;

typedef EulerAngle State;
typedef EulerAngle AngularVelocity;
typedef EulerAngle Acceleration;

void initializeState(State *state);
void estimateByAngularVelocity(State *state, AngularVelocity *omega, Scalar dt, State *result);
void estimateByAcceleration(Acceleration *accl, State *result);
void estimateByComplementaryFilter(State *state, AngularVelocity *omega, Acceleration *accl, Scalar dt, State *result);