/*
 * spi_driver.c
 *
 *  Created on: Dec 30, 2024
 *      Author: st1na
 */

#include "spi_driver.h"

spDevMap_t spiDevTable[MAX_SPI_WRAPPERS] = {0};

void SPIDevRegister(spiDev_t *spiDev, SPI_HandleTypeDef *hspi) {
    for (int i = 0; i < MAX_SPI_WRAPPERS; i++) {
        if (spiDevTable[i].hspi == NULL) {
        	spiDevTable[i].hspi = hspi;
        	spiDevTable[i].spiDev = spiDev;
            return;
        }
    }
    // Error handling if the table is full
    LogMessage(LOG_LEVEL_ERROR, "ERROR: SPI wrapper table is full. Check if MAX_SPI_WRAPPERS is set properly.\n");
}

spiDev_t *SPIDevLookup(SPI_HandleTypeDef *hspi) {
    for (int i = 0; i < MAX_SPI_WRAPPERS; i++) {
        if (spiDevTable[i].hspi == hspi) {
            return spiDevTable[i].spiDev;
        }
    }
    return NULL; // Not found
}

static int Enqueue(spiDev_t *dev, spiRequest_t *request) {
    int nextTail = (dev->queueTail + 1) % MAX_QUEUE_SIZE;
    if (nextTail == dev->queueHead) {
        // Queue is full
        return -1;
    }
    dev->queue[dev->queueTail] = *request;
    dev->queueTail = nextTail;
    return 0;
}

static spiRequest_t *Dequeue(spiDev_t *dev) {
    if (dev->queueHead == dev->queueTail) {
        // Queue is empty
        return NULL;
    }
    spiRequest_t *request = &dev->queue[dev->queueHead];
    dev->queueHead = (dev->queueHead + 1) % MAX_QUEUE_SIZE;
    return request;
}



void SPIInit(spiDev_t *spiDev) {
	if (spiDev->spiConfig.setupMode == SPI_TRANSFER_DMA){
		MX_DMA_Init();
	}
	spiDev->spiConfig.InitFunc();
	spiDev->queueHead = 0;
	spiDev->queueTail = 0;
	spiDev->isBusy = 0;
	spiDev->TxRx = 0;

    SPIDevRegister(spiDev, spiDev->spiConfig.spiHandle);
}


//HAL Callbacks for Interrupt and DMA modes
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi) {
    spiDev_t *dev = SPIDevLookup(hspi);

    if (dev) {
        spiRequest_t *nextRequest = Dequeue(dev);
        if (nextRequest) {
        	if(dev->spiConfig.setupMode == SPI_TRANSFER_DMA){

        		HAL_SPI_Transmit_DMA(dev->spiConfig.spiHandle, nextRequest->txBuffer, nextRequest->size);
        	}
        	else{
        		HAL_SPI_Transmit_IT(dev->spiConfig.spiHandle, nextRequest->txBuffer, nextRequest->size);
            }
        } else {
        	if(dev->TxRx){
        		if(dev->spiConfig.setupMode == SPI_TRANSFER_DMA){
        			HAL_SPI_Receive_DMA(dev->spiConfig.spiHandle, dev->rxBuffer, dev->rxSize);
            	}
            	else{
            		HAL_SPI_Receive_IT(dev->spiConfig.spiHandle, dev->rxBuffer, dev->rxSize);
                }
        	}else{
    			dev->isBusy = 0;
    			dev->TxRx = 0;
        		HAL_GPIO_WritePin(dev->spiConfig.portCS, dev->spiConfig.pinCS, GPIO_PIN_SET);
        	}
        }
    }
}



void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi) {
    spiDev_t *dev = SPIDevLookup(hspi);

    if (dev) {
        spiRequest_t *nextRequest = Dequeue(dev);
        if (nextRequest) {
        	if(dev->spiConfig.setupMode == SPI_TRANSFER_DMA){
        		HAL_SPI_Receive_DMA(dev->spiConfig.spiHandle, nextRequest->rxBuffer, nextRequest->size);
        	}
        	else {
        		HAL_SPI_Receive_IT(dev->spiConfig.spiHandle, nextRequest->rxBuffer, nextRequest->size);
            }
        } else {
        	dev->RxCompleteCallback(dev->rxBuffer);
        	dev->isBusy = 0;
        	dev->TxRx = 0;
            HAL_GPIO_WritePin(dev->spiConfig.portCS, dev->spiConfig.pinCS, GPIO_PIN_SET);
        }
    }
}

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi) {
    spiDev_t *dev = SPIDevLookup(hspi);

    if (dev) {
        spiRequest_t *nextRequest = Dequeue(dev);
        if (nextRequest) {
            HAL_SPI_TransmitReceive_IT(dev->spiConfig.spiHandle, nextRequest->txBuffer, nextRequest->rxBuffer, nextRequest->size);
        } else {
            HAL_GPIO_WritePin(dev->spiConfig.portCS, dev->spiConfig.pinCS, GPIO_PIN_SET);
            dev->TxRxCompleteCallback(dev->rxBuffer);
            dev->isBusy = 0;

        }
    }
}

uint8_t SPITransmit(spiDev_t *dev, uint8_t *data, uint16_t size) {
    if (dev->isBusy) {
        spiRequest_t request = { .txBuffer = data, .rxBuffer = NULL, .size = size};
        return (Enqueue(dev, &request) == 0) ? HAL_BUSY : HAL_ERROR;
    }

	dev->isBusy = 1;
	dev->txBuffer = data;

    switch (dev->spiConfig.setupMode) {
        case SPI_TRANSFER_POLLING:
            HAL_SPI_Transmit(dev->spiConfig.spiHandle, data, size, dev->spiConfig.timeout);
            dev->isBusy = 0;
            break;
        case SPI_TRANSFER_INTERRUPT:
        	HAL_SPI_Transmit_IT(dev->spiConfig.spiHandle, dev->txBuffer, size);
            break;
        case SPI_TRANSFER_DMA:
            HAL_SPI_Transmit_DMA(dev->spiConfig.spiHandle, dev->txBuffer, size);
            break;
    }
    return 1;
}



uint8_t SPIReceive(spiDev_t *dev, uint8_t *data, uint16_t size) {
    if (dev->isBusy) {
        spiRequest_t request = { .txBuffer = NULL, .rxBuffer = data, .size = size};
        return (Enqueue(dev, &request) == 0) ? HAL_BUSY : HAL_ERROR;
    }

	dev->isBusy = 1;
	dev->rxBuffer = data;
	dev->rxSize = size;
    switch (dev->spiConfig.setupMode) {
        case SPI_TRANSFER_POLLING:
            HAL_SPI_Receive(dev->spiConfig.spiHandle, data, size, dev->spiConfig.timeout);
            dev->isBusy = 0;
            break;
        case SPI_TRANSFER_INTERRUPT:
        	HAL_SPI_Receive_IT(dev->spiConfig.spiHandle, dev->rxBuffer, dev->rxSize);
            break;
        case SPI_TRANSFER_DMA:
            HAL_SPI_Receive_DMA(dev->spiConfig.spiHandle, dev->rxBuffer, dev->rxSize);
            break;
    }
    return 1;
}

uint8_t SPITransmitReceive(spiDev_t *dev, uint8_t *txData, uint8_t *rxData, uint16_t size) {
    if (dev->isBusy) {
        spiRequest_t request = { .txBuffer = txData, .rxBuffer = rxData, .size = size};
        return (Enqueue(dev, &request) == 0) ? HAL_BUSY : HAL_ERROR;
    }

    dev->isBusy = 1;
    dev->rxBuffer = rxData;
    dev->txBuffer = txData;
    switch (dev->spiConfig.setupMode) {
        case SPI_TRANSFER_POLLING:
        	HAL_SPI_TransmitReceive(dev->spiConfig.spiHandle, txData, rxData, size, dev->spiConfig.timeout);
        	dev->isBusy = 0;
            break;
        case SPI_TRANSFER_INTERRUPT:
        	HAL_SPI_TransmitReceive_IT(dev->spiConfig.spiHandle,  dev->txBuffer, dev->rxBuffer, size);
            break;
        case SPI_TRANSFER_DMA:
        	HAL_SPI_TransmitReceive_DMA(dev->spiConfig.spiHandle, dev->txBuffer, dev->rxBuffer, size);
            break;
    }
    return 1;
}

