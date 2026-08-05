/*
 * bmp280.c
 *
 *  Created on: Jan 28, 2025
 *      Author: st1na
 */

#include "bmp280.h"

void BMP280_SPIRxCompleteCallback(uint8_t *data) {

}

void BMP280_SPITxCompleteCallback(uint8_t *data) {

}

void BMP280_SPITxRxCompleteCallback(uint8_t *data) {

}

int Bmp280Init (baroDev_t *baroDev){

	baroDev->interface.devInterface->spiDev.spiConfig.setupMode 	= SPI_TRANSFER_POLLING;
	baroDev->interface.devInterface->spiDev.RxCompleteCallback 	= BMP280_SPIRxCompleteCallback;
	baroDev->interface.devInterface->spiDev.TxCompleteCallback 	= BMP280_SPITxCompleteCallback;
	baroDev->interface.devInterface->spiDev.TxRxCompleteCallback = BMP280_SPITxRxCompleteCallback;
	baroDev->interface.devInterface->spiDev.spiConfig.inst 		= BMP280_SPI_INST;
	baroDev->interface.devInterface->spiDev.spiConfig.InitFunc 	= BMP280_SPI_INIT();
	baroDev->interface.devInterface->spiDev.spiConfig.spiHandle 	= BMP280_SPI_HANDLE;
	baroDev->interface.devInterface->spiDev.spiConfig.timeout   	= BMP280_TIMEOUT;
	baroDev->interface.devInterface->spiDev.spiConfig.portCS	   	= BMP280_SPI_CS_PORT;
	baroDev->interface.devInterface->spiDev.spiConfig.pinCS	   	= BMP280_SPI_CS_PIN;

	//TODO: BARO INIT
	baroDev->interface.WriteRegister(BMP280_REG_RESET,(uint8_t*)BMP280_RESET,1);
	HAL_Delay(100);

	return 1;

}


int Bmp280ReadAltitude (baroDev_t *baroDev) {

	//TODO: BARO READ ALTITUDE
	uint8_t data = 0;

	baroDev->interface.ReadRegister(BMP280_REG_ID, &data, 1);
	LogMessage(LOG_LEVEL_DEBUG, "BARO ID 1= %d \r\n", data);
	baroDev->interface.ReadRegister(BMP280_REG_ID, &data, 1);
	LogMessage(LOG_LEVEL_DEBUG, "BARO ID 2= %d \r\n", data);

	return 1;
}
