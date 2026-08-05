/*
 * state_machine.h
 *
 *  Created on: Mar 25, 2026
 *      Author: st1na
 */

#ifndef INC_STATE_MACHINE_H_
#define INC_STATE_MACHINE_H_

#include <stdbool.h>
#include "init.h"
#include "receiver.h"
#include "motor.h"
#include "imu.h"
#include "pid.h"

/*
 * Context struct passed to all state handler functions.
 * Contains only pointers — no data is duplicated.
 * motors[0]=FR  motors[1]=FL  motors[2]=RL  motors[3]=RR
 */
typedef struct {
    RCData_t      *rcData;
    motorDevice_t *motors[4];
    imuAngles_t   *angles;
    pid_t         *rollPID;
    pid_t         *pitchPID;
    pid_t         *yawPID;
    bool           aux2PrevHigh;    /* tracks AUX2 state across all handlers */
    bool           aux2Initialized; /* prevents false trigger on first IBUS frame */
    bool           hasEverArmed;    /* blocks calibration after first arm */
    uint32_t       signalLostTick;   /* 0=signal OK; >0=time Stage 1 started */
} smCtx_t;

armState_e HandleDisarmed(smCtx_t *ctx);
armState_e HandleArmed(smCtx_t *ctx);
armState_e HandleCalibrating(smCtx_t *ctx);
armState_e HandleFailsafe(smCtx_t *ctx);
armState_e HandleCrashDetected(smCtx_t *ctx);

#endif /* INC_STATE_MACHINE_H_ */
