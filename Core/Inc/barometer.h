/*
 * barometer.h
 *
 *  Created on: Jan 28, 2025
 *      Author: st1na
 */

#ifndef INC_BAROMETER_H_
#define INC_BAROMETER_H_

#include "sensor_interface.h"
#include "stdint.h"
#include "init.h"

typedef enum {
	BARO_BMP280
} baroHardware_e;


typedef struct baroOps_s baroOps_t;
typedef struct baroDev_s baroDev_t;

struct baroOps_s {
	int (*Init)(baroDev_t *baroDev);
    int (*ReadAltitude)(baroDev_t *baroDev);
};

struct baroDev_s {
	baroHardware_e gyroHardware;
	baroOps_t ops;
	uint32_t altitude;
	sensorInterface_t interface; //Abstract the protocol used to access the device
};

int BaroInit (baroDev_t *baroDev);
int BariReadAltitude(baroDev_t *baroDev);

#endif /* INC_BAROMETER_H_ */
