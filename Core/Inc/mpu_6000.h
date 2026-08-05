/*
 * mpu_6000.h
 *
 *  Created on: Dec 26, 2024
 *      Author: st1na
 */

#ifndef INC_MPU_6000_H_
#define INC_MPU_6000_H_

#include "stdint.h"
#include "sensor_interface.h"
#include "gyro_acc.h"
#include "init.h"
#include <math.h>
#include "gpio.h"

#define ALPHA 0.90  // Complementary filter constant
#define DT 0.1     // Time step in seconds (sample rate = 100 Hz
#define M_PI 3.14159265358979323846

#define MPU6000_SPI_INIT 	SPI1_INIT
#define MPU6000_SPI_HANDLE  SPI1_HANDLE
#define MPU6000_SPI_INST 	SPI1
#define MPU6000_SPI_CS_PORT	SPI1_CS_PORT
#define MPU6000_SPI_CS_PIN	SPI1_CS_PIN
#define MPU6000_I2C_ADDR	0

#define MPU6000_TIMEOUT 10 //ms for read/write timeout

#define MPU6000_EXTI_PIN GPIO_PIN_4

//MPU6000 REGISTERS
#define MPU6000_REG_SELF_TEST_X    	0x0D
#define MPU6000_REG_SELF_TEST_Y    	0x0E
#define MPU6000_REG_SELF_TEST_Z    	0x0F
#define MPU6000_REG_SELF_TEST_A    	0x10
#define MPU6000_REG_SMPRT_DIV		0x19
#define MPU6000_REG_CONFIG		   	0x1A
#define MPU6000_REG_GYRO_CONFIG	   	0x1B
#define MPU6000_REG_ACCEL_CONFIG   	0x1C
#define MPU6000_REG_FIFO_EN			0x23
#define MPU6000_REG_INT_PIN_CFG		0x37
#define MPU6000_REG_INT_ENABLE  	0x38

#define MPU6000_REG_INT_STATUS		0x3A
#define MPU6000_REG_ACCEL_XOUT_H   	0x3B
#define MPU6000_REG_ACCEL_XOUT_L   	0x3C
#define MPU6000_REG_ACCEL_YOUT_H   	0x3D
#define MPU6000_REG_ACCEL_YOUT_L	0x3E
#define MPU6000_REG_ACCEL_ZOUT_H	0x3F
#define MPU6000_REG_ACCEL_ZOUT_L	0x40
#define MPU6000_REG_TEMP_OUT_H		0x41
#define MPU6000_REG_TEMP_OUT_L		0x42
#define MPU6000_REG_GYRO_XOUT_H		0x43
#define MPU6000_REG_GYRO_XOUT_L		0x44
#define MPU6000_REG_GYRO_YOUT_H		0x45
#define MPU6000_REG_GYRO_YOUT_L		0x46
#define MPU6000_REG_GYRO_ZOUT_H		0x47
#define MPU6000_REG_GYRO_ZOUT_L		0x48

#define MPU6000_REG_SIGNAL_PATH_RES	0x68
#define MPU6000_REG_USER_CTRL		0x6A
#define MPU6000_REG_PWR_MGMT_1	   	0x6B
#define MPU6000_REG_PWR_MGMT_2	   	0x6C
#define MPU6000_REG_WHO_AM_I       	0x75

#define MPU6000_REG_WHO_AM_I_DEFAULT_VALUE 	0x68

#define MPU6000_DEVICE_RESET 0x80
#define MPU6000_GYRO_ACC_TEMP_RESET 0x07

//GYRo CONFIG
#define MPU6000_GYRO_FS_250DPS     0x00  // ±250 °/s
#define MPU6000_GYRO_FS_500DPS     0x01  // ±500 °/s
#define MPU6000_GYRO_FS_1000DPS    0x02  // ±1000 °/s
#define MPU6000_GYRO_FS_2000DPS    0x03  // ±2000 °/s

#define MPU6000_GYRO_FS_SEL_VAL    MPU6000_GYRO_FS_2000DPS // Set desired gyro range range
#define MPU6000_GYRO_FS_SEL        (MPU6000_GYRO_FS_SEL_VAL << 3)

#define MPU6000_GYRO_X_SELF_TEST (1 << 7)     // Enable X-axis gyro self-test
#define MPU6000_GYRO_Y_SELF_TEST (1 << 6)     // Enable Y-axis gyro self-test
#define MPU6000_GYRO_Z_SELF_TEST (1 << 5)     // Enable Z-axis gyro self-test

//ACCEL CONFIG
#define MPU6000_ACCEL_AFS_2G_VAL        0x00  // ±2 g
#define MPU6000_ACCEL_AFS_4G_VAL        0x01  // ±4 g
#define MPU6000_ACCEL_AFS_8G_VAL        0x02  // ±8 g
#define MPU6000_ACCEL_AFS_16G_VAL       0x03  // ±16 g

#define MPU6000_ACCEL_AFS_SEL_VAL		MPU6000_ACCEL_AFS_16G_VAL // Set desired accel range
#define MPU6000_ACCEL_AFS_SEL           (MPU6000_ACCEL_AFS_SEL_VAL << 3)

#define MPU6000_ACCEL_X_SELF_TEST (1 << 7)    // Enable X-axis accel self-test
#define MPU6000_ACCEL_Y_SELF_TEST (1 << 6)    // Enable Y-axis accel self-test
#define MPU6000_ACCEL_Z_SELF_TEST (1 << 5)    // Enable Z-axis accel self-test

int Mpu6000Init (gyroAccDev_t *gyroAcc);
gyroRawAccData_t Mpu6000ReadRawGyroAccData (gyroAccDev_t *gyroAcc);
int Mpu6000CalibrateGyroAcc (gyroAccDev_t *gyroAcc);
gyroAccData_t Mpu6000ReadGyroAcc (gyroAccDev_t *gyroAcc);


#endif /* INC_MPU_6000_H_ */
