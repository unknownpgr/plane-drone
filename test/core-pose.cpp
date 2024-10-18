#include "core-pose.hpp"
#include <math.h>

void initializeState(State *state)
{
  state->pitch = 0.0;
  state->roll = 0.0;
  state->yaw = 0.0;
}

void estimateByAngularVelocity(State *state, AngularVelocity *omega, Scalar dt, State *result)
{
  Scalar roll = state->roll;
  Scalar pitch = state->pitch;
  Scalar omega_roll = omega->roll;
  Scalar omega_pitch = omega->pitch;
  Scalar omega_yaw = omega->yaw;

  Scalar dot_roll = omega_roll + sin(roll) * tan(pitch) * omega_pitch + cos(roll) * tan(pitch) * omega_yaw;
  Scalar dot_pitch = cos(roll) * omega_pitch - sin(roll) * omega_yaw;
  Scalar dot_yaw = (sin(roll) / cos(pitch)) * omega_pitch + (cos(roll) / cos(pitch)) * omega_yaw;

  result->roll = roll + dot_roll * dt;
  result->pitch = pitch + dot_pitch * dt;
  result->yaw = state->yaw + dot_yaw * dt;
}

void estimateByAcceleration(Acceleration *accl, State *result)
{
  Scalar a_x = accl->roll;
  Scalar a_y = accl->pitch;
  Scalar a_z = accl->yaw;

  result->pitch = atan2(-a_x, sqrt(a_y * a_y + a_z * a_z));
  result->roll = atan2(a_y, a_z);
}

void estimateByComplementaryFilter(State *state, AngularVelocity *omega, Acceleration *accl, Scalar dt, State *result)
{
  const float gain = 0.95f;

  State tmp;
  estimateByAngularVelocity(state, omega, dt, &tmp);
  estimateByAcceleration(accl, result);

  result->roll = gain * tmp.roll + (1.f - gain) * result->roll;
  result->pitch = gain * tmp.pitch + (1.f - gain) * result->pitch;
  result->yaw = gain * tmp.yaw + (1.f - gain) * result->yaw;
}