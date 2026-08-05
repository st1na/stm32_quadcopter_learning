/*
 * mpu_6000.c
 *
 *  Created on: Dec 26, 2024
 *      Author: st1na
 */

#include "mpu_6000.h"

static gyroAccDev_t *gyroAccDev_int;

//TODO: CLEAN UPD MPU600 and GYRODEV code

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	//LogMessage(LOG_LEVEL_DEBUG, "INT FIRE \r\n");
    if (GPIO_Pin == MPU6000_EXTI_PIN) {
    	static uint8_t data[14] = {0};
    	if(!gyroAccDev_int->interface.BusBusy()){
    		gyroAccDev_int->interface.ReadRegister(MPU6000_REG_ACCEL_XOUT_H, data, 14);
    	} else {
    		//LogMessage(LOG_LEVEL_DEBUG, "BUSY \r\n");
    	}
    }
}

void MPU6000_SPIRxCompleteCallback(uint8_t *data) {

//	float gsx = 0, gsy = 0, gsz = 0, asx = 0, asy = 0, asz = 0;
	if(gyroAccDev_int->requestData == 0){

		//READ DATA
		gyroAccDev_int->rawData.acc[X] = data[0] << 8 | data[1];
		gyroAccDev_int->rawData.acc[Y] = data[2] << 8 | data[3];
		gyroAccDev_int->rawData.acc[Z] = data[4] << 8 | data[5];

		gyroAccDev_int->rawData.gyro[X] = data[8] << 8 | data[9];
		//TODO: Fix this to be configurable from init
		//Invert y axis as the sensor is mounted with y pointing backwards
		gyroAccDev_int->rawData.gyro[Y] = -1*(data[10] << 8 | data[11]);
		gyroAccDev_int->rawData.gyro[Z] = data[12] << 8 | data[13];

		//SCALED DATA
		gyroAccDev_int->scaledData.gyro[X] = (float)(gyroAccDev_int->rawData.gyro[X] - gyroAccDev_int->bias.gyro[X])/gyroAccDev_int->gyroScale;
		gyroAccDev_int->scaledData.gyro[Y] = (float)(gyroAccDev_int->rawData.gyro[Y] - gyroAccDev_int->bias.gyro[Y])/gyroAccDev_int->gyroScale;
		gyroAccDev_int->scaledData.gyro[Z] = (float)(gyroAccDev_int->rawData.gyro[Z] - gyroAccDev_int->bias.gyro[Z])/gyroAccDev_int->gyroScale;

		gyroAccDev_int->scaledData.acc[X] = (float)(gyroAccDev_int->rawData.acc[X])/gyroAccDev_int->accScale;
		gyroAccDev_int->scaledData.acc[Y] = (float)(gyroAccDev_int->rawData.acc[Y])/gyroAccDev_int->accScale;
		gyroAccDev_int->scaledData.acc[Z] = (float)(gyroAccDev_int->rawData.acc[Z])/gyroAccDev_int->accScale;


//		gsx = (float)(gyroAccDev_int->rawData.gyro[X] - gyroAccDev_int->bias.gyro[X])/gyroAccDev_int->gyroScale;
//		gsy = (float)(gyroAccDev_int->rawData.gyro[Y] - gyroAccDev_int->bias.gyro[Y])/gyroAccDev_int->gyroScale;
//		gsz = (float)(gyroAccDev_int->rawData.gyro[Z] - gyroAccDev_int->bias.gyro[Z])/gyroAccDev_int->gyroScale;
//
//		asx = (float)(gyroAccDev_int->rawData.acc[X])/gyroAccDev_int->accScale;
//		asy = (float)(gyroAccDev_int->rawData.acc[Y])/gyroAccDev_int->accScale;
//		asz = (float)(gyroAccDev_int->rawData.acc[Z])/gyroAccDev_int->accScale;
//
//		//CAL ANGLES
//		gyroAccDev_int->anglesInDeg.acc[Z] = atanf(sqrt(asy*asy + asx*asx)/asz);
//		gyroAccDev_int->anglesInDeg.acc[Y] = atan2f(asx,sqrtf(asy*asy + asz*asz));
//		gyroAccDev_int->anglesInDeg.acc[X] = atan2f(asy,sqrtf(asx*asx + asz*asz));
//
//		gyroAccDev_int->anglesInDeg.acc[Z] *= 180/M_PI;
//		gyroAccDev_int->anglesInDeg.acc[Y] *= 180/M_PI;
//		gyroAccDev_int->anglesInDeg.acc[X] *= 180/M_PI;
//
//		gyroAccDev_int->anglesInDeg.gyro[X] += gsx*gyroAccDev_int->sampleTime_ms/1000;
//		gyroAccDev_int->anglesInDeg.gyro[Y] += gsy*gyroAccDev_int->sampleTime_ms/1000;
//		gyroAccDev_int->anglesInDeg.gyro[Z] += gsz*gyroAccDev_int->sampleTime_ms/1000;

	}
}

void MPU6000_SPITxCompleteCallback(uint8_t *data) {

}

void MPU6000_SPITxRxCompleteCallback(uint8_t *data) {

}

int Mpu6000Init (gyroAccDev_t *gyroAcc){

	gyroAcc->interface.devInterface->spiDev.spiConfig.setupMode 	= SPI_TRANSFER_DMA;
	gyroAcc->interface.devInterface->spiDev.RxCompleteCallback 	= MPU6000_SPIRxCompleteCallback;
	gyroAcc->interface.devInterface->spiDev.TxCompleteCallback 	= MPU6000_SPITxCompleteCallback;
	gyroAcc->interface.devInterface->spiDev.TxRxCompleteCallback = MPU6000_SPITxRxCompleteCallback;
	gyroAcc->interface.devInterface->spiDev.spiConfig.inst 		= MPU6000_SPI_INST;
	gyroAcc->interface.devInterface->spiDev.spiConfig.InitFunc 	= MPU6000_SPI_INIT();
	gyroAcc->interface.devInterface->spiDev.spiConfig.spiHandle 	= MPU6000_SPI_HANDLE;
	gyroAcc->interface.devInterface->spiDev.spiConfig.timeout   	= MPU6000_TIMEOUT;
	gyroAcc->interface.devInterface->spiDev.spiConfig.portCS	   	= MPU6000_SPI_CS_PORT;
	gyroAcc->interface.devInterface->spiDev.spiConfig.pinCS	   	= MPU6000_SPI_CS_PIN;


	for(int i=0; i<NUM_AXIS; i++){
		gyroAcc->rawData.acc[i] = 0;
		gyroAcc->rawData.gyro[i] = 0;
		gyroAcc->bias.acc[i] = 0;
		gyroAcc->bias.gyro[i] = 0;
		gyroAcc->scaledData.acc[i] = 0;
		gyroAcc->scaledData.gyro[i] = 0;
	}

	gyroAcc->interface.InitProtocol(gyroAcc->interface.devInterface);
	gyroAccDev_int = gyroAcc;
	gyroAcc->requestData = 0;
	gyroAcc->calibrationDone = 0;
	static const double gyroRanges[] = {131.0, 65.5, 32.8, 16.4};
	gyroAcc->gyroRange = 250 *(2 << (MPU6000_GYRO_FS_SEL_VAL-1));
	gyroAcc->accRange  = 2<<(MPU6000_ACCEL_AFS_SEL_VAL);
	gyroAcc->gyroScale = gyroRanges[MPU6000_GYRO_FS_SEL_VAL]*180.0/M_PI;
	gyroAcc->accScale  = 2 <<(13 - MPU6000_ACCEL_AFS_SEL_VAL);

	// Reset device
	gyroAcc->interface.WriteRegister(MPU6000_REG_PWR_MGMT_1, (uint8_t*)MPU6000_DEVICE_RESET, 1);
	HAL_Delay(100);
	gyroAcc->interface.WriteRegister(MPU6000_REG_PWR_MGMT_1, (uint8_t*)0x00, 1);
	HAL_Delay(100);
	//Reset gyro accel and temp
	gyroAcc->interface.WriteRegister(MPU6000_REG_SIGNAL_PATH_RES, (uint8_t*)MPU6000_GYRO_ACC_TEMP_RESET, 1);
	HAL_Delay(100);
	gyroAcc->interface.WriteRegister(MPU6000_REG_SIGNAL_PATH_RES, (uint8_t*)0x00, 1);
	HAL_Delay(100);


	uint8_t gyroConfig = MPU6000_GYRO_FS_SEL;
	uint8_t accConfig = MPU6000_ACCEL_AFS_SEL;
	//TODO: remove hardcoded values and use defines or some struct to hold the mpu6000 config values
	uint8_t smprt_div = 0x00;
	uint8_t config = 0x02;
	gyroAcc->sampleTime_ms = (smprt_div+1)/1; // Gyro 1KHZ and ACC 1KHz


	gyroAcc->interface.WriteRegister(MPU6000_REG_SMPRT_DIV, &smprt_div, 1);
	HAL_Delay(20);
	gyroAcc->interface.WriteRegister(MPU6000_REG_CONFIG, &config ,1);
	HAL_Delay(20);
	gyroAcc->interface.WriteRegister(MPU6000_REG_GYRO_CONFIG, &gyroConfig ,1);
	HAL_Delay(20);
	gyroAcc->interface.WriteRegister(MPU6000_REG_ACCEL_CONFIG, &accConfig ,1);
	HAL_Delay(20);

	LogMessage(LOG_LEVEL_INFO, "GYRO ACCELEROMETER CALIBRATION START\r\n");
	gyroAcc->ops.CalibrateGyroAcc(gyroAcc);
	LogMessage(LOG_LEVEL_INFO, "GYRO ACCELEROMETER CALIBRATION END\r\n");

	// Enable interrupt after callibration is Done
	uint8_t inpPinCfg = 0x10; //INT_LEVEL = 0, INT push pull with 50us INT pulse, clear when any register is read
	uint8_t intEnable = 0x01; // Enable data_rdy_en interrupt only
	gyroAcc->interface.WriteRegister(MPU6000_REG_INT_PIN_CFG, &inpPinCfg ,1);
	HAL_Delay(20);
	gyroAcc->interface.WriteRegister(MPU6000_REG_INT_ENABLE, &intEnable ,1);
	HAL_Delay(20);

	return 1;
};

gyroRawAccData_t Mpu6000ReadRawGyroAccData (gyroAccDev_t *gyroAcc){

	uint8_t data[14] = {0};
	gyroRawAccData_t rawData = {0};

	if(gyroAcc->interface.devInterface->spiDev.spiConfig.setupMode  == SPI_TRANSFER_POLLING){
		gyroAcc->interface.ReadRegister(MPU6000_REG_ACCEL_XOUT_H, data, 14);
		gyroAcc->rawData.acc[X] = data[0] << 8 | data[1];
		gyroAcc->rawData.acc[Y] = data[2] << 8 | data[3];
		gyroAcc->rawData.acc[Z] = data[4] << 8 | data[5];

		gyroAcc->rawData.gyro[X] = data[8] << 8 | data[9];
		gyroAcc->rawData.gyro[Y] = data[10] << 8 | data[11];
		gyroAcc->rawData.gyro[Z] = data[12] << 8 | data[13];
	}
	else{
		gyroAcc->requestData = 1;
		rawData = gyroAcc->rawData;
		gyroAcc->requestData = 0;
	}
	rawData = gyroAcc->rawData;
	return rawData;
}


int Mpu6000CalibrateGyroAcc (gyroAccDev_t *gyroAcc) {

	int32_t gyroBias[NUM_AXIS] = {0};
	int32_t accBias[NUM_AXIS] = {0};
	uint8_t data[14] = {0};

	//read wait for new sample
	for(int i = 0; i < CAL_SAMPLES; i++){
		//Mpu6000ReadRawGyroAccData(gyroAcc);
		if(!gyroAcc->interface.BusBusy()){
			gyroAcc->interface.ReadRegister(MPU6000_REG_ACCEL_XOUT_H, data, 14);
		};
		//HAL_Delay(1);
		while(gyroAcc->interface.BusBusy()){}
		gyroBias[X] += gyroAcc->rawData.gyro[X];
		gyroBias[Y] += gyroAcc->rawData.gyro[Y];
		gyroBias[Z] += gyroAcc->rawData.gyro[Z];
		accBias[X] += gyroAcc->rawData.acc[X];
		accBias[Y] += gyroAcc->rawData.acc[Y];
		accBias[Z] += gyroAcc->rawData.acc[Z];

		//LogMessage(LOG_LEVEL_DEBUG, "RAW_GYRO_X:%d RAW_GYRO_Y:%d RAW_GYRO_Z:%d RAW_ACC_X:%d RAW_ACC_Y:%d RAW_ACC_Z:%d\r\n",
				//gyroAcc->rawData.gyro[X] , gyroAcc->rawData.gyro[Y] , gyroAcc->rawData.gyro[Z] , gyroAcc->rawData.acc[X], gyroAcc->rawData.acc[Y], gyroAcc->rawData.acc[Z]);
	}

	gyroAcc->bias.gyro[X] = (int16_t)(gyroBias[X]/CAL_SAMPLES);
	gyroAcc->bias.gyro[Y] = (int16_t)(gyroBias[Y]/CAL_SAMPLES);
	gyroAcc->bias.gyro[Z] = (int16_t)(gyroBias[Z]/CAL_SAMPLES);

	gyroAcc->bias.acc[X] = (int16_t)(accBias[X]/CAL_SAMPLES);
	gyroAcc->bias.acc[Y] = (int16_t)(accBias[Y]/CAL_SAMPLES);
	gyroAcc->bias.acc[Z] = (int16_t)(accBias[Z]/CAL_SAMPLES);
	gyroAcc->calibrationDone = 1;
	return 1;
}

gyroAccData_t Mpu6000ReadGyroAcc (gyroAccDev_t *gyroAcc) {
	gyroAccData_t scaledData = {0};
	uint8_t data[14] = {0};
	if(gyroAcc->interface.devInterface->spiDev.spiConfig.setupMode  == SPI_TRANSFER_POLLING){
		gyroAcc->interface.ReadRegister(MPU6000_REG_ACCEL_XOUT_H, data, 14);
		gyroAcc->rawData.acc[X] = data[0] << 8 | data[1];
		gyroAcc->rawData.acc[Y] = data[2] << 8 | data[3];
		gyroAcc->rawData.acc[Z] = data[4] << 8 | data[5];

		gyroAcc->rawData.gyro[X] = data[8] << 8 | data[9];
		gyroAcc->rawData.gyro[Y] = data[10] << 8 | data[11];
		gyroAcc->rawData.gyro[Z] = data[12] << 8 | data[13];

		gyroAcc->scaledData.gyro[X] = (float)(gyroAcc->rawData.gyro[X] - gyroAcc->bias.gyro[X])/gyroAcc->gyroScale;
		gyroAcc->scaledData.gyro[Y] = (float)(gyroAcc->rawData.gyro[Y] - gyroAcc->bias.gyro[Y])/gyroAcc->gyroScale;
		gyroAcc->scaledData.gyro[Z] = (float)(gyroAcc->rawData.gyro[Z] - gyroAcc->bias.gyro[Z])/gyroAcc->gyroScale;

		gyroAcc->scaledData.acc[X] = (float)(gyroAcc->rawData.acc[X])/gyroAcc->accScale;
		gyroAcc->scaledData.acc[Y] = (float)(gyroAcc->rawData.acc[Y])/gyroAcc->accScale;
		gyroAcc->scaledData.acc[Z] = (float)(gyroAcc->rawData.acc[Z])/gyroAcc->accScale;
		scaledData = gyroAcc->scaledData;
	}
	else{
		gyroAcc->requestData = 1;
		scaledData = gyroAcc->scaledData;
		gyroAcc->requestData = 0;
	}

	return scaledData;
}

