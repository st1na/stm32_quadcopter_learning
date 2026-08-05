/*
 * imu.c
 *
 *  Created on: Feb 23, 2025
 *      Author: st1na
 */

#include "imu.h"

//TODO: imu calculate data create setup which supports different filters and clean up code wiht proper data passing
imuAngles_t IMUCalculateAngles(gyroAccData_t gyroAccData, imuFilter_e filterType){
	imuAngles_t imuAnglesFiltered = {0};
	float gx_rad = gyroAccData.gyro[X] * (M_PI / 180.0f);
	float gy_rad = gyroAccData.gyro[Y] * (M_PI / 180.0f);
	float gz_rad = gyroAccData.gyro[Z] * (M_PI / 180.0f);
	float ax = gyroAccData.acc[X];
	float ay = gyroAccData.acc[Y];
	float az = gyroAccData.acc[Z];
#ifdef USE_COMPASS
	switch(filterType) {
	case MADGWICK:
		// Call Madgwick fuilter with comp
		break;
	case KALMAN:
		// Call Kalman filter with comp
		break;
	default:
		break;
	}
#else
	switch(filterType) {
	case MADGWICK:
		Quaternion r = {0};
		// Call Madgwick fuilter without comp
		r = MadgwickAHRSupdateIMU(gx_rad, gy_rad, gz_rad, ax, ay, az);
/*		r = MadgwickAHRSupdateIMU(
		    gyroAccData.gyro[X],
		    gyroAccData.gyro[Y],
		    gyroAccData.gyro[Z],
		    gyroAccData.acc[X],
		    gyroAccData.acc[Y],
		    gyroAccData.acc[Z]
		);
*/
		//LogMessage(LOG_LEVEL_DEBUG, "q0:%.5f q1:%.5f q2:%.5f q3:%.5f\r\n", q0, q1, q2, q3);
		//LogMessage(LOG_LEVEL_DEBUG, "SAMPLE FREQ= %d", sampleFreq);
		// Euler angles calc from quaternion in rad
		double roll, pitch, yaw;
		// Roll (x-axis rotation)
		roll = atan2(2.0 * (r.w*r.x + r.y*r.z), 1.0 - 2.0 * (r.x*r.x + r.y*r.y));
		// Pitch (y-axis rotation)
		double sinp = 2.0 * (r.w*r.y - r.z*r.x);
		if (fabs(sinp) >= 1)
		    pitch = copysign(M_PI / 2, sinp); // use 90 degrees if out of range
		else
		    pitch = asin(sinp);
		// Yaw (z-axis rotation)
		yaw = atan2(2.0 * (r.w*r.z + r.x*r.y), 1.0 - 2.0 * (r.y*r.y + r.z*r.z));
		//

		imuAnglesFiltered.q = r;
		imuAnglesFiltered.rpyRad[ROLL] = roll;
		imuAnglesFiltered.rpyRad[YAW] = yaw;
		imuAnglesFiltered.rpyRad[PITCH] = pitch;
		imuAnglesFiltered.rpyDeg[ROLL] = roll*180.0/M_PI;
		imuAnglesFiltered.rpyDeg[YAW] = yaw*180.0/M_PI;
		imuAnglesFiltered.rpyDeg[PITCH] = pitch*180.0/M_PI;

		break;
	case KALMAN:
		// Call Kalman filter without comp
		break;
	default:
		break;
	}
#endif

	return imuAnglesFiltered;
};

//Timer interrupt at fixed sample period set to 3ms from tim.c TIM1 CH1 settings
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM1 &&
        htim->Channel  == HAL_TIM_ACTIVE_CHANNEL_1)
    {
    	__FLAGS |= FLAG_TIMER_IMU;

    }
}
