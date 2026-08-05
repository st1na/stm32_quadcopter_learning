/*
 * pid.c
 *
 *  Created on: Jan 2, 2026
 *      Author: st1na
 */

#include "pid.h"

static inline float ConstrainValue(float value, float minValue, float maxValue)
{
    if (value < minValue) return minValue;
    if (value > maxValue) return maxValue;
    return value;
}

void PidInit(pid_t *pid,
             float kp,
             float ki,
             float kd,
             float outMin,
             float outMax)
{
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->integral = 0.0f;
    pid->prevMeasurement = 0.0f;
    pid->outMin = outMin;
    pid->outMax = outMax;
}

float PidUpdate(pid_t *pid,
                float targetValue,
                float measuredValue,
                float deltaTime)
{
    float errorValue = targetValue - measuredValue;

    // integral term with anti-windup
    pid->integral += errorValue * deltaTime;
    pid->integral = ConstrainValue(pid->integral, pid->outMin, pid->outMax);

    // derivative on measurement
    float derivativeValue =
        (measuredValue - pid->prevMeasurement) / deltaTime;

    float outputValue =
          pid->kp * errorValue
        + pid->ki * pid->integral
        - pid->kd * derivativeValue;

    pid->prevMeasurement = measuredValue;

    return ConstrainValue(outputValue, pid->outMin, pid->outMax);
}

void PidReset(pid_t *pid)
{
    pid->integral = 0.0f;
    pid->prevMeasurement = 0.0f;
}
