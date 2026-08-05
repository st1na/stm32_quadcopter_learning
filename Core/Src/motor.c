/*
 * motor.c
 *
 *  Created on: Feb 23, 2024
 *      Author: st1na
 */

#include "motor.h"
#include "pwm.h"
#include "log.h"

static const motorVtable_t mottorVtableNULL = {
    .MotorInit = MotorInitNULL,
	.MotorEnable = MotorEnableNULL,
	.MotorWrite = MotorWriteNULL,
	.MotorDisable = MotorDisableNULL
};


/* Motor device initialization helper with null checks */
motorDevice_t* InitMotor(TIM_HandleTypeDef *timer, uint32_t channel) {
    motorDevice_t *motor = MotorDeviceInit(PWM);
    if (!motor) {
        LogMessage(LOG_LEVEL_ERROR, "Motor allocation failed\r\n");
        return NULL;
    }
    motor->timerHandle = timer;
    motor->timerChannel = channel;
    motor->vTable->MotorInit(motor);
    return motor;
}

motorDevice_t *MotorDeviceInit(t_ESCProtocol ESCProtocol){
	motorDevice_t *motorDevice = malloc(sizeof(motorDevice_t));
	if (!motorDevice) {
		return NULL;
	}
	memset(motorDevice,0,sizeof(motorDevice_t));
    switch (ESCProtocol){
    	case PWM:
    		motorDevice->vTable = &mottorVtablePWM;
    		break;
    	default:
    		motorDevice->vTable = &mottorVtableNULL;
    }
    return motorDevice;
};

void MotorDeviceDeInit(motorDevice_t *motorDevice) {
	if (motorDevice) {
		free(motorDevice);
	}
};

void MotorInitNULL (motorDevice_t *motorDevice) {

};

void MotorEnableNULL (motorDevice_t *motorDevice) {

};

void MotorWriteNULL (motorDevice_t *motorDevice, uint32_t pulseWidth) {

};

void MotorDisableNULL (motorDevice_t *motorDevice) {

};

/*
 * ESC Throttle Calibration Sequence
 *
 * Must be called before ArmESCs. Sends 100% throttle so the ESC captures
 * the maximum endpoint, then drops to 0% so the ESC captures the minimum,
 * saves the range and completes arming (1 high beep).
 *
 * Sequence per BLHeli_S/32 arming spec:
 * 1. 2000 us (100%) held for 2s -> ESC beeps low, runs calibration capture
 * 2. 1000 us (0%)   held for 2s -> ESC saves range, beeps high (armed)
 */
void CalibrateESCs(motorDevice_t *motors[4]) {
	if (!motors || !motors[0] || !motors[1] || !motors[2] || !motors[3]) {
		LogMessage(LOG_LEVEL_ERROR, "CalibrateESCs: invalid motor pointers\r\n");
		return;
	}

	LogMessage(LOG_LEVEL_INFO, "Start ESC calibration\r\n");

	LogMessage(LOG_LEVEL_INFO, "ESC calibration: sending min throttle...\r\n");
	for (int i = 0; i < 4; i++) {
		motors[i]->vTable->MotorWrite(motors[i], 1000);
	}
	HAL_Delay(500);

	LogMessage(LOG_LEVEL_INFO, "ESC calibration: sending max throttle...\r\n");
	for (int i = 0; i < 4; i++) {
		motors[i]->vTable->MotorWrite(motors[i], 2000);
	}
	HAL_Delay(2000);

	LogMessage(LOG_LEVEL_INFO, "ESC calibration: sending min throttle...\r\n");
	for (int i = 0; i < 4; i++) {
		motors[i]->vTable->MotorWrite(motors[i], 1000);
	}
	HAL_Delay(2000);

	LogMessage(LOG_LEVEL_INFO, "ESC calibration complete\r\n");
}

/*
 * ESC Arming Sequence
 * 
 * Sends minimum throttle (1000 PWM) to all ESCs to initialize them.
 * This must be called before the main control loop starts.
 * 
 * Standard ESC initialization:
 * 1. Apply minimum throttle (1000 PWM) to all motors
 * 2. Wait 2-3 seconds for ESC calibration and beep confirmation
 * 3. Then proceed with normal flight control
 */
void ArmESCs(motorDevice_t *motors[4]) {
	if (!motors || !motors[0] || !motors[1] || !motors[2] || !motors[3]) {
		LogMessage(LOG_LEVEL_ERROR, "Invalid motor device pointers\r\n");
		return;
	}

	LogMessage(LOG_LEVEL_INFO, "Sending minimum throttle to ESCs...\r\n");

	/* Send minimum throttle (1000 PWM) to all 4 motors */
	for (int i = 0; i < 4; i++) {
		motors[i]->vTable->MotorWrite(motors[i], 1000);
	}



	LogMessage(LOG_LEVEL_INFO, "ESCs armed successfully\r\n");
}

/*
 * ESC Disarm Function
 * 
 * Sends minimum throttle (1000 PWM) to all ESCs to disarm them.
 * Should be called when the user switches off the arming switch.
 */
void DisarmESCs(motorDevice_t *motors[4]) {
	if (!motors || !motors[0] || !motors[1] || !motors[2] || !motors[3]) {
		LogMessage(LOG_LEVEL_ERROR, "Invalid motor device pointers\r\n");
		return;
	}

	LogMessage(LOG_LEVEL_INFO, "Disarming ESCs - sending minimum throttle\r\n");

	/* Send minimum throttle (1000 PWM) to all 4 motors */
	for (int i = 0; i < 4; i++) {
		motors[i]->vTable->MotorWrite(motors[i], 1000);
	}

	LogMessage(LOG_LEVEL_INFO, "ESCs disarmed\r\n");
}


