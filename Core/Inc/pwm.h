/*
 * pwm.h
 *
 *  Created on: Nov 23, 2024
 *      Author: st1na
 */

#ifndef INC_PWM_H_
#define INC_PWM_H_

#include "motor.h"
#include "tim.h"

void MotorInitPWM (motorDevice_t *);
void MotorEnablePWM (motorDevice_t *);
void MotorWritePWM (motorDevice_t *, uint32_t pulseWidth);
void MotorDisablePWM (motorDevice_t *);

static const motorVtable_t mottorVtablePWM = {
    .MotorInit = MotorInitPWM,
	.MotorEnable = MotorEnablePWM,
	.MotorWrite = MotorWritePWM,
	.MotorDisable = MotorDisablePWM
};



#endif /* INC_PWM_H_ */
