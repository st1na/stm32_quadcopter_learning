/*
 * receiver.c
 *
 *  Created on: Dec 19, 2024
 *      Author: st1na
 */

#include "receiver.h"

RCData_t* RCInit (t_RCProtocol rcProtocol){

	RCData_t *rcData = malloc(sizeof(RCData_t));
	memset(rcData,0,sizeof(RCData_t));

	switch (rcProtocol){
		case IBUS:
	    	IbusInit();
	    	rcData->rcProtocol = IBUS;
	    	rcData->rcInputMin = 1000;
	    	rcData->rcInputMid = 1500;
	    	rcData->rcInputMax = 2000;
	    	break;
	    case EXPRESSLRS:
	    	rcData->rcProtocol = EXPRESSLRS;
	    	break;
	}

	return rcData;
};

void RCRead(RCData_t* rcData){

	switch (rcData->rcProtocol){
		case IBUS:
	    	IbusRead(rcData->rawRcInput);
	    	RCMapIbus(rcData);
	    	break;
	    case EXPRESSLRS:
	    	break;
	}
};

void RCMapIbus (RCData_t* rcData){
	//[0]-row [1]-pitch [3]-yaw [2]-alt
	//ROLL = CH[0] PITCH = CH[1] THROTTLE = CH[2] YAW = CH[3] AUX1 = CH[4] AUX2 = CH[5]
	rcData->rcInput[RC_ROLL] = rcData->rawRcInput[0];
	rcData->rcInput[RC_PITCH] = rcData->rawRcInput[1];
	rcData->rcInput[RC_THROTTLE] = rcData->rawRcInput[2];
	rcData->rcInput[RC_YAW] = rcData->rawRcInput[3];
	rcData->rcInput[RC_AUX1] = rcData->rawRcInput[4];
	rcData->rcInput[RC_AUX2] = rcData->rawRcInput[5];
};

bool RCSignalLost(RCData_t *rcData){
	switch (rcData->rcProtocol){
		case IBUS:  return IbusSignalLost(IBUS_SIGNAL_TIMEOUT_MS);
		default:    return false;
	}
}
