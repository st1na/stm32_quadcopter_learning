/*
 * motor.h
 *
 *  Created on: Feb 23, 2024
 *      Author: st1na
 */

#ifndef INC_MOTOR_H_
#define INC_MOTOR_H_

#include "stdlib.h"
#include "stdio.h"
#include "stdint.h"
#include "stdbool.h"
#include "string.h"
#include "tim.h"


typedef enum  {
	PWM,
	ONESHOT125,
	ONESHOT42,
	MULTISHOT,
	DSHOT150,
	DSHOT300,
	DSHOT600
} t_ESCProtocol;

typedef struct motorVtable_s motorVtable_t;
typedef struct motorDevice_s motorDevice_t;

struct motorVtable_s {
	void (*MotorInit)(motorDevice_t *);
	void (*MotorEnable)(motorDevice_t *);
	void (*MotorWrite)(motorDevice_t *, uint32_t);
	void (*MotorDisable)(motorDevice_t *);

};

struct motorDevice_s {
	bool initialized;
	bool enabled;
	TIM_HandleTypeDef* timerHandle;
	uint8_t timerChannel;
	const motorVtable_t *vTable;

};

motorDevice_t *MotorDeviceInit(t_ESCProtocol);

motorDevice_t *InitMotor(TIM_HandleTypeDef *timer, uint32_t channel);

void MotorDeviceDeInit(motorDevice_t *);

void CalibrateESCs(motorDevice_t *motors[4]);

void ArmESCs(motorDevice_t *motors[4]);

void DisarmESCs(motorDevice_t *motors[4]);

void MotorInitNULL (motorDevice_t *);
void MotorEnableNULL (motorDevice_t *);
void MotorWriteNULL (motorDevice_t *, uint32_t);
void MotorDisableNULL (motorDevice_t *);



#endif /* INC_MOTOR_H_ */
