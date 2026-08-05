/*
 * sensors.h
 *
 *  Created on: Mar 3, 2024
 *      Author: st1na
 */

#ifndef INC_SENSOR_INTERFACE_H_
#define INC_SENSOR_INTERFACE_H_

#include "stdint.h"
#include "spi.h"
#include "gpio.h"
#include "log.h"
#include "stdbool.h"
#include "spi_driver.h"

typedef enum {
	SENSOR_OK,
	SENSOR_ERROR_TIMEOUT,
	SENSOR_ERROR_BUSY,
	SENSOR_ERROR_UNKNOWN
} sensorStatus_e;

typedef enum {
	SPI,
	I2C
} sensorProtocol_e;

typedef union devInterface_u{
    spiDev_t spiDev;  // SPI Device
    //i2cDev_t i2cDev;  // I2C Device
} devInterface_t;


// InitProtocol is set by the user of the interface for I2C or SPI
typedef struct sensorInterface_s {
	void (*InitProtocol)(devInterface_t *devInterface);
	sensorStatus_e (*ReadRegister)(uint8_t reg, uint8_t *data, uint16_t len);
	sensorStatus_e (*WriteRegister)(uint8_t reg, uint8_t *data, uint16_t len);
	uint8_t	(*BusBusy)(void);
	devInterface_t *devInterface;
} sensorInterface_t;

void SPIInitProtocol(devInterface_t *devInterface);
sensorStatus_e SPIReadRegister (uint8_t reg, uint8_t *data, uint16_t len);
sensorStatus_e SPIWriteRegister ( uint8_t reg, uint8_t *data, uint16_t len);
uint8_t SPIBusy(void);

sensorStatus_e I2CReadRegister (uint8_t reg, uint8_t *data, uint16_t len);
sensorStatus_e I2CWriteRegister (uint8_t reg, uint8_t *data, uint16_t len);

#endif /* INC_SENSOR_INTERFACE_H_ */
