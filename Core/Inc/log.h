/*
 * log.h
 *
 *  Created on: Jan 24, 2024
 *      Author: st1na
 */

#ifndef INC_LOG_H_
#define INC_LOG_H_

#include "usb_device.h"
#include "usbd_cdc_if.h"
#include "stdbool.h"
#include "stdio.h"
#include "stdarg.h"

#define LOG_ENABLE 1
#define LOG_LEVEL LOG_LEVEL_DEBUG
#define LOG_BUF_SIZE 2048
#define MESSAGE_SIZE 96

typedef enum {
	LOG_LEVEL_NONE,
	LOG_LEVEL_ERROR,
	LOG_LEVEL_WARNING,
	LOG_LEVEL_INFO,
	LOG_LEVEL_DEBUG
} logLevel_e;

typedef struct logDev_s {
	void (*init)(void);
	void (*write)(const char* message, size_t length);
} logDev_t;

typedef struct logRingBuffer_s
{
    uint8_t  buf[LOG_BUF_SIZE];
    uint16_t head;
    uint16_t tail;
    uint8_t  busy;
} logRingBuffer_t;

extern volatile logRingBuffer_t USBRingBuffer;

//void LogInit(void);
void LogEnable(void);

#if LOG_ENABLE
void _LogMessage(logLevel_e level, const char *format, ...);
#define LogMessage(level, format, ...) \
	do { if ((level) <= LOG_LEVEL) _LogMessage(level, format, ##__VA_ARGS__); } while(0)
#else
#define LogMessage(level, format, ...) ((void)0)
#endif

// USB LOG

void UsbWrite(const char* message, size_t len);
void UsbDrain(void);

#endif /* INC_LOG_H_ */
