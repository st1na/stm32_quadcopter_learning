/*
 * barometer.c
 *
 *  Created on: Jan 28, 2025
 *      Author: st1na
 */

#include "barometer.h"

int BaroInit (baroDev_t *baroDev) {
	if(baroDev && baroDev->ops.Init){
		return baroDev->ops.Init(baroDev);
	}
	return -1;
}

int BariReadAltitude(baroDev_t *baroDev) {
	if(baroDev && baroDev->ops.ReadAltitude){
		return baroDev->ops.ReadAltitude(baroDev);
	}
	return -1;
}
