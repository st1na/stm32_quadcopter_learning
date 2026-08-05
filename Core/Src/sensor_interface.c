/*
 * sensors.c
 *
 *  Created on: Mar 3, 2024
 *      Author: st1na
 */
#define CYCLES_PER_US (SystemCoreClock / 100000)
#include "sensor_interface.h"

static spiDev_t *spiDev_int;


void SPIInitProtocol(devInterface_t *devInterface){
	spiDev_int = &devInterface->spiDev;
	SPIInit(spiDev_int);
};


sensorStatus_e SPIReadRegister (uint8_t reg, uint8_t *data, uint16_t len){

	uint8_t regRead = reg | 0x80;

	spiDev_int->TxRx = 1;
	spiDev_int->rxBuffer = data;
	spiDev_int->txBuffer = &reg;
	spiDev_int->rxSize	 = len;
	//spiDev_int->isBusy = 1;

	HAL_GPIO_WritePin(spiDev_int->spiConfig.portCS, spiDev_int->spiConfig.pinCS, GPIO_PIN_RESET);
	SPITransmit(spiDev_int,&regRead, 1);
	if(spiDev_int->spiConfig.setupMode == SPI_TRANSFER_POLLING){
		SPIReceive(spiDev_int,data, len);
		HAL_GPIO_WritePin(spiDev_int->spiConfig.portCS, spiDev_int->spiConfig.pinCS, GPIO_PIN_SET);
	}
	//For interrupt and DMA mode CS is SET in Interrupt
	return 1;
}


sensorStatus_e SPIWriteRegister (uint8_t reg, uint8_t *data, uint16_t len){

	uint8_t txData[len+1];
	txData[0] = reg & 0b01111111;
	memcpy(&txData[1], data, len);

	spiDev_int->TxRx = 0;
	spiDev_int->txBuffer = txData;
	spiDev_int->rxSize	 = 0;
	//spiDev_int->isBusy = 1;

	HAL_GPIO_WritePin(spiDev_int->spiConfig.portCS, spiDev_int->spiConfig.pinCS, GPIO_PIN_RESET);
	SPITransmit(spiDev_int,txData, len+1);

	if(spiDev_int->spiConfig.setupMode == SPI_TRANSFER_POLLING){
		HAL_GPIO_WritePin(spiDev_int->spiConfig.portCS, spiDev_int->spiConfig.pinCS, GPIO_PIN_SET);
	}
	//For interrupt and DMA mode CS is SET in Interrupt
	return 1;
}

uint8_t SPIBusy(void){
	return spiDev_int->isBusy;
}
