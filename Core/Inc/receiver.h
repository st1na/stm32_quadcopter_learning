/*
 * receiver.h
 *
 *  Created on: Dec 19, 2024
 *      Author: st1na
 *      Wrapper for different RX protocols
 */

#ifndef INC_RECEIVER_H_
#define INC_RECEIVER_H_

#include "ibus.h"

#define RC_CHANNELS 6

typedef enum  {
	IBUS,
	EXPRESSLRS
} t_RCProtocol;

typedef enum  {
	RC_ROLL,
	RC_PITCH,
	RC_YAW,
	RC_THROTTLE,
	RC_AUX1,
	RC_AUX2
} t_RCInput;

typedef struct RCData_s {
	t_RCProtocol rcProtocol;
	uint16_t rawRcInput[RC_CHANNELS];
	uint16_t rcInput[RC_CHANNELS];
	uint16_t rcInputMin;
	uint16_t rcInputMid;
	uint16_t rcInputMax;
} RCData_t;

RCData_t* RCInit (t_RCProtocol);
void RCRead(RCData_t*);
void RCMapIbus(RCData_t*);
bool RCSignalLost(RCData_t *rcData);

#endif /* INC_RECEIVER_H_ */
