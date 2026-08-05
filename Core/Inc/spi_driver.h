/*
 * spi_driver.h
 *
 *  Created on: Dec 30, 2024
 *      Author: st1na
 */

#ifndef INC_SPI_DRIVER_H_
#define INC_SPI_DRIVER_H_

#include <stdint.h>
#include <stddef.h>
#include <spi.h>
#include "log.h"
#include "dma.h"

#define MAX_SPI_WRAPPERS 3
#define MAX_QUEUE_SIZE 200
// Global lookup table for SPI wrappers


// SPI Transfer Modes
typedef enum {
    SPI_TRANSFER_POLLING,
    SPI_TRANSFER_INTERRUPT,
    SPI_TRANSFER_DMA
} SPITransferMode;


// SPI Configuration Struct
typedef struct spiConfig_s{
	SPI_TypeDef *	   inst;
    SPI_HandleTypeDef* spiHandle;
    SPITransferMode setupMode; //
    void (*InitFunc) (void);
    uint32_t		   timeout;
	GPIO_TypeDef * portCS;
	uint16_t	pinCS;
} spiConfig_t;

typedef void (*SPICallback)(uint8_t *data);

typedef struct spiRequest_s{
	uint8_t *txBuffer;
	uint8_t *rxBuffer;
	uint16_t size;
} spiRequest_t;

typedef struct spiDev_s{
	spiConfig_t spiConfig;
	spiRequest_t queue[MAX_QUEUE_SIZE];
	uint8_t queueHead;
	uint8_t queueTail;
	uint8_t isBusy;
	uint8_t *txBuffer;
	uint8_t *rxBuffer;
	uint16_t rxSize;
    SPICallback TxCompleteCallback;
    SPICallback RxCompleteCallback;
    SPICallback TxRxCompleteCallback;
    uint8_t TxRx;
} spiDev_t;

typedef struct spDevMap_s{
    SPI_HandleTypeDef *hspi;
    spiDev_t *spiDev;
} spDevMap_t;


void SPIDevRegister(spiDev_t *spiDev, SPI_HandleTypeDef *hspi);
spiDev_t *SPIDevLookup(SPI_HandleTypeDef *hspi);
void SPIInit(spiDev_t *spiDev);
uint8_t SPITransmit(spiDev_t *dev, uint8_t *data, uint16_t size);
uint8_t SPIReceive(spiDev_t *dev, uint8_t *data, uint16_t size);
uint8_t SPITransmitReceive(spiDev_t *dev, uint8_t *txData, uint8_t *rxData, uint16_t size);




#endif /* INC_SPI_DRIVER_H_ */
