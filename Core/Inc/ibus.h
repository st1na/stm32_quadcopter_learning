/*
 * ibus.h
 *
 *  Created on: Jan 20, 2024
 *      Author: st1na
 */

#ifndef INC_IBUS_H_
#define INC_IBUS_H_

#include <log.h>
#include "stdint.h"
#include "stdbool.h"
#include "usart.h"
#include "dma.h"

#define IBUS_UART (&huart1)
#define IBUS_USER_CHANNELS 6
#define IBUS_LENGTH 0x20
#define IBUS_COMMAND40          0x40
#define IBUS_SIGNAL_TIMEOUT_MS  100U

struct ibusRxData{
	uint8_t 	len;
	uint8_t 	code;
	uint16_t 	ch[IBUS_USER_CHANNELS];
	uint16_t	checkSum;

};

bool IbusRead (uint16_t *ibusData);
void IbusInit (void);

void IbusUpdate (void);
bool IbusCheckSum(void);
bool IbusValid(void);
bool IbusSignalLost(uint32_t timeoutMs);

#endif /* INC_IBUS_H_ */
