/*
 * bmp280.h
 *
 *  Created on: Jan 28, 2025
 *      Author: st1na
 */

#ifndef INC_BMP280_H_
#define INC_BMP280_H_

#include "stdint.h"
#include "sensor_interface.h"
#include "barometer.h"
#include "init.h"
#include <math.h>
#include "gpio.h"

#define BMP280_SPI_INIT 	SPI3_INIT
#define BMP280_SPI_HANDLE   SPI3_HANDLE
#define BMP280_SPI_INST 	SPI3
#define BMP280_SPI_CS_PORT	BARO_CS_PORT
#define BMP280_SPI_CS_PIN	BARO_CS_PIN
#define BMP280_I2C_ADDR	0

#define BMP280_TIMEOUT 10 //ms for read/write timeout

#define BMP280_REG_RESET 0xE0
#define BMP280_REG_ID 0xD0

#define BMP280_RESET 0xB6

int Bmp280Init (baroDev_t *baroDev);
int Bmp280ReadAltitude (baroDev_t *baroDev);

#endif /* INC_BMP280_H_ */
