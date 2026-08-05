/*
 * log.c
 *
 *  Created on: Jan 24, 2024
 *      Author: st1na
 */

#include "log.h"

static const logDev_t logDev = {
	.init = MX_USB_DEVICE_Init,
	.write = UsbWrite
};

volatile logRingBuffer_t USBRingBuffer = {0};
// Static buffer: persists across function calls, safe for async DMA transfers
static uint8_t packet[MESSAGE_SIZE];

void LogEnable(void){
	logDev.init();
}


#if LOG_ENABLE
void _LogMessage(logLevel_e level, const char *format, ...) {
    uint32_t ms = HAL_GetTick();
    char buffer[MESSAGE_SIZE];

    int prefix_len = snprintf(buffer, sizeof(buffer), "[%4lu.%03lu] ",
                              ms / 1000, ms % 1000);

    va_list args;
    va_start(args, format);
    vsnprintf(buffer + prefix_len, sizeof(buffer) - prefix_len, format, args);
    va_end(args);
    logDev.write(buffer, strlen(buffer));
}
#endif




void UsbDrain(void) {
    if (USBRingBuffer.busy) return;
    if (USBRingBuffer.head == USBRingBuffer.tail) return;

    uint16_t tx_len = 0;
    while ((USBRingBuffer.tail != USBRingBuffer.head) && (tx_len < sizeof(packet)))
    {
        packet[tx_len++] = USBRingBuffer.buf[USBRingBuffer.tail];
        USBRingBuffer.tail = (USBRingBuffer.tail + 1) % LOG_BUF_SIZE;
    }

    uint8_t usbRes = CDC_Transmit_FS(packet, tx_len);
    if (usbRes == USBD_OK)
    {
        USBRingBuffer.busy = 1;
    }
    else if (usbRes == USBD_BUSY)
    {
        // Roll back tail to retry next time
        USBRingBuffer.tail = (USBRingBuffer.tail + LOG_BUF_SIZE - tx_len) % LOG_BUF_SIZE;
    }
}

void UsbWrite(const char* message, size_t len) {
    // Write to ring buffer
    if (message && len > 0)
    {
        for (size_t i = 0; i < len; i++)
        {
            uint16_t next = (USBRingBuffer.head + 1) % LOG_BUF_SIZE;
            if (next == USBRingBuffer.tail)
            {
                // Overwrite oldest byte
                USBRingBuffer.tail = (USBRingBuffer.tail + 1) % LOG_BUF_SIZE;
            }
            USBRingBuffer.buf[USBRingBuffer.head] = message[i];
            USBRingBuffer.head = next;
        }
    }

    if (__get_IPSR() != 0) return; // skip if called from ISR — TransmitCplt will drain
    UsbDrain();
}



