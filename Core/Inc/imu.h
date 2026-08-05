/*
 * imu.h
 *
 *  Created on: Feb 23, 2025
 *      Author: st1na
 */

#ifndef INC_IMU_H_
#define INC_IMU_H_

#include "init.h"
#include "gyro_acc.h"
#include "compass.h"
#include "MadgwickAHRS.h"
#include "math.h"
#include "stdbool.h"
// Take data from sensors gyro/acc and mag if available and apply filters to data

typedef struct imuAngles_s imuAngles_t;

struct imuAngles_s {
	Quaternion q;
	float rpyRad[NUM_AXIS];
	float rpyDeg[NUM_AXIS];
	bool imuReady;
};


imuAngles_t IMUCalculateAngles(gyroAccData_t gyroAccData, imuFilter_e filterType);


#endif /* INC_IMU_H_ */
