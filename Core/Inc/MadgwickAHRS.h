//=====================================================================================================
// MadgwickAHRS.h
//=====================================================================================================
//
// Implementation of Madgwick's IMU and AHRS algorithms.
// See: http://www.x-io.co.uk/node/8#open_source_ahrs_and_imu_algorithms
//
// Date			Author          Notes
// 29/09/2011	SOH Madgwick    Initial release
// 02/10/2011	SOH Madgwick	Optimised for reduced CPU load
//
//=====================================================================================================
#ifndef MadgwickAHRS_h
#define MadgwickAHRS_h

#include "init.h"
//---------------------------------------------------------------------------------------------------
// Definitions
#define sampleFreq	(1.0f/IMU_FILTER_DT) 		// sample frequency in Hz if this is changes the TIM1 CH1 Period and Prescaler have to be changed accordingly
//TODO: Create a global define macro for sampleFreq setting and prescaler Period setting
#define betaDef		0.95f		// 2 * proportional gain

//----------------------------------------------------------------------------------------------------
// Variable declaration
typedef struct {
    float w;
    float x;
    float y;
    float z;
} Quaternion;

//---------------------------------------------------------------------------------------------------
// Function declarations

Quaternion MadgwickAHRSupdate(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz);
Quaternion MadgwickAHRSupdateIMU(float gx, float gy, float gz, float ax, float ay, float az);

#endif
//=====================================================================================================
// End of file
//=====================================================================================================
