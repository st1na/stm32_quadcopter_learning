/*
 * pwm.c
 *
 *  Created on: Nov 23, 2024
 *      Author: st1na
 */


#include "pwm.h"

/* PWM Configuration Parameters */
#define PWM_PRESCALER 71
#define PWM_PERIOD 4000

void MotorInitPWM (motorDevice_t *motorDevice) {
	  /* Configure timer parameters BEFORE starting PWM */
	  motorDevice->timerHandle->Init.Prescaler = PWM_PRESCALER;
	  motorDevice->timerHandle->Init.CounterMode = TIM_COUNTERMODE_UP;
	  motorDevice->timerHandle->Init.Period = PWM_PERIOD;

	  /* Apply the new configuration.
	   * HAL_TIM_Base_Init always ends with State = READY, so the subsequent
	   * HAL_TIM_Base_Start and HAL_TIM_PWM_Start succeed even when two motors
	   * share the same timer (e.g. FR and RR both use TIM3). */
	  HAL_TIM_Base_Init(motorDevice->timerHandle);

	  /* Start the base counter, then enable the PWM output channel */
	  HAL_TIM_Base_Start(motorDevice->timerHandle);
	  HAL_TIM_PWM_Start(motorDevice->timerHandle, motorDevice->timerChannel);
};

void MotorEnablePWM (motorDevice_t *motorDevice) {

};

void MotorWritePWM (motorDevice_t *motorDevice, uint32_t pulseWidth) {
	__HAL_TIM_SET_COMPARE(motorDevice->timerHandle, motorDevice->timerChannel,pulseWidth);
};

void MotorDisablePWM (motorDevice_t *motorDevice) {

};
