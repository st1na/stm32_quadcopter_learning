/*
 * gyro.c
 *
 *  Created on: Dec 25, 2024
 *      Author: st1na
 */

#include "gyro_acc.h"

gyroAccDevMap_t gyroAccDevMap;

void GyroAccDevExtIntRegister (gyroAccDev_t *gyroAcc, uint16_t gpioPin){
	gyroAccDevMap.gyroAccDev = gyroAcc;
	gyroAccDevMap.extiPin = gpioPin;
}

gyroAccDev_t *GyroAccDevExtIntLookup (uint16_t gpioPin){
	return gyroAccDevMap.gyroAccDev;
}

int GyroAccInit (gyroAccDev_t *gyroAcc) {
	if(gyroAcc && gyroAcc->ops.Init){
		return gyroAcc->ops.Init(gyroAcc);
	}
	return -1;
}

gyroRawAccData_t GyroAccReadRawGyroAccData (gyroAccDev_t *gyroAcc) {
	gyroRawAccData_t result = {0};
	if(gyroAcc && gyroAcc->ops.ReadRawGyroAccData){
		return gyroAcc->ops.ReadRawGyroAccData(gyroAcc);
	}
	return result;
}

int GyroAccCalibrateGyroAcc (gyroAccDev_t *gyroAcc) {
	if(gyroAcc && gyroAcc->ops.CalibrateGyroAcc){
		return gyroAcc->ops.CalibrateGyroAcc(gyroAcc);
	}
	return -1;
}

gyroAccData_t GyroAccReadGyroAcc(gyroAccDev_t *gyroAcc) {

	gyroAccData_t result = {0};

	if(gyroAcc && gyroAcc->ops.ReadGyroAcc){
		return gyroAcc->ops.ReadGyroAcc(gyroAcc);
	}
	return result;
}



