/*
 * gyro.h
 *
 *  Created on: Dec 25, 2024
 *      Author: st1na
 */

#ifndef INC_GYRO_ACC_H_
#define INC_GYRO_ACC_H_

#include "sensor_interface.h"
#include "stdint.h"
#include "init.h"

#define NUM_AXIS 3
#define CAL_SAMPLES 1000

typedef enum {
	GYRO_ACC_MPU6500
} gyroAccHardware_e;



typedef struct gyroRawAccData_s gyroRawAccData_t;
typedef struct gyroAccData_s gyroAccData_t;
typedef struct gyroAccOps_s gyroAccOps_t;
typedef struct gyroAccDev_s gyroAccDev_t;
typedef struct gyroAccDevMap_s gyroAccDevMap_t;

struct gyroRawAccData_s {
	int16_t gyro[NUM_AXIS];
	int16_t acc[NUM_AXIS];
};

struct gyroAccData_s {
	float gyro[NUM_AXIS];
	float acc[NUM_AXIS];
};

struct gyroAccOps_s {
	int (*Init)(gyroAccDev_t *gyroAcc);
	gyroRawAccData_t (*ReadRawGyroAccData)(gyroAccDev_t *gyroAcc);
    gyroAccData_t (*ReadGyroAcc)(gyroAccDev_t *gyroAcc);
    int (*CalibrateGyroAcc)(gyroAccDev_t *gyroAcc);
    int (*ReadAngles)(gyroAccDev_t *gyroAcc);
};

struct gyroAccDev_s {
	gyroAccHardware_e gyroHardware;
	gyroRawAccData_t rawData; //Raw data from IMU registers
	gyroRawAccData_t bias;	  //Bias from calibration
	gyroAccData_t scaledData; // Data scaled according to the scale factor and converted to gyro[deg/s] accel[g]
	gyroAccOps_t ops;
	sensorInterface_t interface; //Abstract the protocol used to access the device
	uint16_t gyroRange;
	uint16_t accRange;
	float gyroScale;
	uint16_t accScale;
	uint8_t calibrationDone;
	double sampleTime_ms; // sample time in micro seconds
	uint8_t requestData;

};

struct gyroAccDevMap_s {
	gyroAccDev_t *gyroAccDev;
    uint16_t extiPin;
};

int GyroAccInit (gyroAccDev_t *gyroAcc);
int GyroAccCalibrateGyroAcc (gyroAccDev_t *gyroAcc);
gyroRawAccData_t GyroAccReadRawGyroAccData (gyroAccDev_t *gyroAcc);
gyroAccData_t GyroAccReadGyroAcc (gyroAccDev_t *gyroAcc);
void GyroAccDevExtIntRegister (gyroAccDev_t *gyroAcc, uint16_t gpioPin);
gyroAccDev_t *GyroAccDevExtIntLookup (uint16_t gpioPin);

#endif /* INC_GYRO_ACC_H_ */
