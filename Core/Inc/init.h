/*
 * init.h
 *
 *  Created on: Nov 24, 2024
 *      Author: st1na
 */

#ifndef INC_INIT_H_
#define INC_INIT_H_

#include "stdint.h"

#define NUMBER_OF_MOTORS 4
#define SPI1_INIT() 	(MX_SPI1_Init)
#define SPI1_HANDLE 	&hspi1
#define SPI1_CS_PORT	GPIOA
#define SPI1_CS_PIN		GPIO_PIN_4

#define SPI3_INIT() 	(MX_SPI3_Init)
#define SPI3_HANDLE 	&hspi3
#define SPI3_CS_PORT	GPIOA
#define SPI3_CS_PIN		GPIO_PIN_15
#define BARO_CS_PORT	GPIOB
#define BARO_CS_PIN		GPIO_PIN_3

#define DEBUG_PORT GPIOC
#define DEBUG_PIN  GPIO_PIN_6

//#define USE_COMPASS

typedef enum {
	X,
	Y,
	Z
} axis_e;

typedef enum {
	ROLL,
	PITCH,
	YAW
} rotation_e;

typedef enum {
	MADGWICK,
	KALMAN
} imuFilter_e;

#define TIMER_CLOCK      72000000UL             // Timer clock in Hz
#define IMU_FILTER_DT    0.003f                 // Desired sample period in seconds
// Choose a small PSC to maximize resolution
#define IMU_TIMER_PSC    3
// Compute ARR automatically from dt and PSC
#define IMU_TIMER_ARR    ((uint32_t)((TIMER_CLOCK * IMU_FILTER_DT) / (IMU_TIMER_PSC + 1)) - 1)

extern volatile uint32_t __FLAGS;
#define FLAG_TIMER_IMU  (1<<0)

/* PID control loop timing (333Hz = 3ms sampling rate) */
#define PID_CONTROL_DT 0.003f

/* Throttle dead zone: motors are stopped when the raw RC throttle channel
 * is at or below this value. Set above the transmitter's physical minimum
 * (typically 1000-1050) to guarantee the stop condition is reached even
 * if the TX/trim outputs slightly above 1000 at the bottom of the stick. */
#define THROTTLE_DEAD_ZONE  1050

/* Arm state machine */
#define AUX_ARM_HIGH            1750  /* AUX1 above this µs = arm request */
#define AUX_ARM_LOW             1250  /* AUX1 below this µs = disarm request */
#define AUX_CAL_HIGH            1750  /* AUX2 above this µs = ESC calibration */
#define ARM_DEBOUNCE_THRESHOLD  166   /* ~500ms at 333Hz loop rate */

typedef enum {
    DISARMED        = 0,
    PREARMING       = 1,
    ARMED           = 2,
    CALIBRATING     = 3,
    FAILSAFE        = 4,
    CRASH_DETECTED  = 5,
} armState_e;

#define FAILSAFE_STAGE1_MS  500U   /* Stage 1 hold duration before disarm */

#endif /* INC_INIT_H_ */
