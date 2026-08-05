/*
 * pid.h
 *
 *  Created on: Jan 2, 2026
 *      Author: st1na
 */

#ifndef INC_PID_H_
#define INC_PID_H_

#include "stdint.h"

/* PID controller structure with anti-windup and output saturation */
typedef struct {
    float kp;               /* Proportional gain */
    float ki;               /* Integral gain */
    float kd;               /* Derivative gain */
    
    float integral;         /* Accumulated integral term (state) */
    float prevMeasurement;  /* Previous measured value for derivative calculation */
    float outMin;           /* Output minimum limit */
    float outMax;           /* Output maximum limit */
} pid_t;

/* 
 * Initialize PID controller with gains and output limits
 * @param pid: PID controller instance
 * @param kp: Proportional gain
 * @param ki: Integral gain
 * @param kd: Derivative gain
 * @param outMin: Output minimum limit
 * @param outMax: Output maximum limit
 */
void PidInit(pid_t *pid, float kp, float ki, float kd, float outMin, float outMax);

/* 
 * Update PID controller and return control output
 * Should be called at fixed time intervals (deltaTime must be consistent)
 * @param pid: PID controller instance
 * @param targetValue: Desired setpoint value
 * @param measuredValue: Current measured value from sensor
 * @param deltaTime: Time since last call in seconds (e.g., 0.003f for 333Hz)
 * @return: PID output value (constrained between outMin and outMax)
 */
float PidUpdate(pid_t *pid, float targetValue, float measuredValue, float deltaTime);

/* 
 * Reset PID controller state (integral and previous measurement)
 * Use when disarming or switching control modes
 * @param pid: PID controller instance
 */
void PidReset(pid_t *pid);

#endif /* INC_PID_H_ */
