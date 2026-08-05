/*
 * control.h
 *
 *  Created on: Feb 27, 2026
 *      Author: st1na
 */

#ifndef INC_CONTROL_H_
#define INC_CONTROL_H_

#include "motor.h"
#include "log.h"
/*
 * Motor mixer for X-quad configuration
 * Converts attitude control outputs (roll, pitch, yaw) and throttle
 * to individual motor commands and outputs to ESCs
 *
 * @param roll: Roll rate/angle correction from PID (-1 to 1)
 * @param pitch: Pitch rate/angle correction from PID (-1 to 1)
 * @param yaw: Yaw rate/angle correction from PID (-1 to 1)
 * @param throttle: Throttle command (0 to 1)
 * @param motorDevice_FR: Front-Right motor device
 * @param motorDevice_FL: Front-Left motor device
 * @param motorDevice_RL: Rear-Left motor device
 * @param motorDevice_RR: Rear-Right motor device
 */
void MotorMixerUpdate(float roll, float pitch, float yaw, float throttle,
                      motorDevice_t *motorDevice_FR,
                      motorDevice_t *motorDevice_FL,
                      motorDevice_t *motorDevice_RL,
                      motorDevice_t *motorDevice_RR);

/* Future control functions:
 * - void ArmDisarmUpdate(...)      // Arming logic based on RC
 * - void FlightModeUpdate(...)     // Flight mode selection
 * - void FailsafeUpdate(...)       // Loss of signal handling
 * - void RateLimiterUpdate(...)    // Rate limiting for smooth control
 */

#endif /* INC_CONTROL_H_ */
