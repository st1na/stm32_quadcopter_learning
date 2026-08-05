/*
 * ibus.c
 *
 *  Created on: Jan 20, 2024
 *      Author: st1na
 */

#include <ibus.h>
#include <stdatomic.h>

static struct ibusRxData ibusRxData;
static uint8_t rawRxData[32] = {0};
//static atomic_uint_fast8_t newFrameAvailable = 0;
static uint32_t lastValidFrameTick = 0;

#define IBUS_FRAME_TIMEOUT_MS 100


void IbusInit (void){
	MX_DMA_Init();
	MX_USART1_UART_Init();
	HAL_UART_Receive_DMA(IBUS_UART,rawRxData,sizeof(rawRxData));

};

bool IbusRead (uint16_t *ibusData){

	/*if (!atomic_exchange_explicit(&newFrameAvailable, 0, memory_order_acq_rel)) {
		LogMessage(LOG_LEVEL_DEBUG, "IBUS: No new frame\r\n");
		return false;
	}*/
	IbusUpdate();

	if(!IbusValid()){
	    LogMessage(LOG_LEVEL_DEBUG, "IBUS: Invalid frame\r\n");
	    //Restart uart
	    IbusInit();
	    IbusUpdate();
	    return false;
	}

	if(!IbusCheckSum()){
	    LogMessage(LOG_LEVEL_DEBUG, "IBUS: Checksum fail\r\n");
	    return false;
	}

	for(int i=0; i<IBUS_USER_CHANNELS; i++){
		ibusData[i] = ibusRxData.ch[i];
	};

	LogMessage(LOG_LEVEL_DEBUG, "IBUS RAW: %d %d %d %d %d %d\r\n",
    	ibusData[0], ibusData[1], ibusData[2], ibusData[3], ibusData[4], ibusData[5]);

	lastValidFrameTick = HAL_GetTick();
	return true;
}

bool IbusSignalLost(uint32_t timeoutMs){
	if (lastValidFrameTick == 0) return false;
	return (HAL_GetTick() - lastValidFrameTick) > timeoutMs;
}

void IbusUpdate (void){
	ibusRxData.len = rawRxData[0];
	ibusRxData.code = rawRxData[1];
	for(int i=0; i<IBUS_USER_CHANNELS; i++){
		ibusRxData.ch[i] = rawRxData[(2*i)+3] << 8 | rawRxData[(2*i)+2];
	};
	ibusRxData.checkSum = rawRxData[31] << 8 | rawRxData[30];
};

bool IbusCheckSum(void){
	uint16_t checkSumCal = 0xFFFF;


	for(int i=0; i<30; i++){
		checkSumCal -= rawRxData[i];
	};

	return(checkSumCal == ibusRxData.checkSum);
};

bool IbusValid(void){

	return (ibusRxData.len == IBUS_LENGTH && ibusRxData.code == IBUS_COMMAND40);
};

// UART interrupt callback
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){

	//HAL_UART_Receive_IT(&huart1,uart1_rx_data,32);
	HAL_UART_Receive_DMA(IBUS_UART,rawRxData,sizeof(rawRxData));
	//atomic_store_explicit(&newFrameAvailable, 1, memory_order_release);

}
