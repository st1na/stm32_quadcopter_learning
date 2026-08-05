/*
 * state_machine.c
 *
 *  Created on: Mar 25, 2026
 *      Author: st1na
 */

#include "state_machine.h"
#include "control.h"
#include "log.h"
#include <stdbool.h>

armState_e HandleDisarmed(smCtx_t *ctx) {
    LogMessage(LOG_LEVEL_DEBUG, "STATE: DISARMED\r\n");
    static uint16_t armCounter = 0;

    /* On first call, prime aux2PrevHigh from actual switch state so a switch
     * already in the UP position at boot does not trigger calibration. */
    if (!ctx->aux2Initialized) {
        ctx->aux2PrevHigh   = (ctx->rcData->rcInput[RC_AUX2] > AUX_CAL_HIGH);
        ctx->aux2Initialized = true;
    }

    /* AUX2 rising edge → transition to CALIBRATING (only allowed before first arm) */
    if (!ctx->hasEverArmed && ctx->rcData->rcInput[RC_AUX2] > AUX_CAL_HIGH && !ctx->aux2PrevHigh) {
        ctx->aux2PrevHigh = true;
        return CALIBRATING;
    }
    if (ctx->rcData->rcInput[RC_AUX2] <= AUX_CAL_HIGH) {
        ctx->aux2PrevHigh = false;
    }

    /* AUX1 high + throttle below dead zone → debounce to ARMED */
    if (ctx->rcData->rcInput[RC_AUX1] > AUX_ARM_HIGH &&
        ctx->rcData->rcInput[RC_THROTTLE] < THROTTLE_DEAD_ZONE) {
        if (++armCounter >= ARM_DEBOUNCE_THRESHOLD) {
            armCounter = 0;
            LogMessage(LOG_LEVEL_INFO, "ARMING\r\n");
            ArmESCs(ctx->motors);
            return ARMED;
        }
    } else {
        armCounter = 0;
    }

    return DISARMED;
}

armState_e HandleArmed(smCtx_t *ctx) {
    LogMessage(LOG_LEVEL_DEBUG, "STATE: ARMED\r\n");
    static uint16_t disarmCounter = 0;

    ctx->hasEverArmed = true;
    /* Keep aux2PrevHigh in sync while armed so re-entry to DISARMED sees the correct state */
    ctx->aux2PrevHigh = (ctx->rcData->rcInput[RC_AUX2] > AUX_CAL_HIGH);

    /* Signal loss → enter FAILSAFE state */
    if (RCSignalLost(ctx->rcData)) return FAILSAFE;

    /* AUX1 low → debounce to DISARMED */
    if (ctx->rcData->rcInput[RC_AUX1] < AUX_ARM_LOW) {
        if (++disarmCounter >= ARM_DEBOUNCE_THRESHOLD) {
            disarmCounter = 0;
            LogMessage(LOG_LEVEL_INFO, "DISARMING\r\n");
            DisarmESCs(ctx->motors);
            PidReset(ctx->rollPID);
            PidReset(ctx->pitchPID);
            PidReset(ctx->yawPID);
            return DISARMED;
        }
    } else {
        disarmCounter = 0;
    }

    /* Flight control */
    float roll_sp  = (ctx->rcData->rcInput[RC_ROLL]     - 1500.0f) / 500.0f * 45.0f;
    float pitch_sp = (ctx->rcData->rcInput[RC_PITCH]    - 1500.0f) / 500.0f * 45.0f;
    float yaw_sp   = (ctx->rcData->rcInput[RC_YAW]      - 1500.0f) / 500.0f * 180.0f;
    float throttle = (ctx->rcData->rcInput[RC_THROTTLE] - 1000.0f) / 1000.0f;

    if (ctx->rcData->rcInput[RC_THROTTLE] > THROTTLE_DEAD_ZONE) {
        float roll  = PidUpdate(ctx->rollPID,  roll_sp,  ctx->angles->rpyDeg[ROLL],  PID_CONTROL_DT) / 500.0f;
        float pitch = PidUpdate(ctx->pitchPID, pitch_sp, ctx->angles->rpyDeg[PITCH], PID_CONTROL_DT) / 500.0f;
        float yaw   = PidUpdate(ctx->yawPID,   yaw_sp,   ctx->angles->rpyDeg[YAW],   PID_CONTROL_DT) / 500.0f;
        MotorMixerUpdate(roll, pitch, yaw, throttle,
                         ctx->motors[0], ctx->motors[1],
                         ctx->motors[2], ctx->motors[3]);
    } else {
        PidReset(ctx->rollPID);
        PidReset(ctx->pitchPID);
        PidReset(ctx->yawPID);
        ctx->motors[0]->vTable->MotorWrite(ctx->motors[0], 1000);
        ctx->motors[1]->vTable->MotorWrite(ctx->motors[1], 1000);
        ctx->motors[2]->vTable->MotorWrite(ctx->motors[2], 1000);
        ctx->motors[3]->vTable->MotorWrite(ctx->motors[3], 1000);
    }

    return ARMED;
}

armState_e HandleCalibrating(smCtx_t *ctx) {
    LogMessage(LOG_LEVEL_INFO, "ESC CALIBRATION START\r\n");
    CalibrateESCs(ctx->motors);
    LogMessage(LOG_LEVEL_INFO, "ESC CALIBRATION DONE\r\n");
    return DISARMED;
}

armState_e HandleFailsafe(smCtx_t *ctx) {
    LogMessage(LOG_LEVEL_DEBUG, "STATE: FAILSAFE\r\n");
    if (ctx->signalLostTick == 0) {
        ctx->signalLostTick = HAL_GetTick();
        LogMessage(LOG_LEVEL_ERROR, "FAILSAFE: RC SIGNAL LOST\r\n");
    }

    ctx->motors[0]->vTable->MotorWrite(ctx->motors[0], 1000);
    ctx->motors[1]->vTable->MotorWrite(ctx->motors[1], 1000);
    ctx->motors[2]->vTable->MotorWrite(ctx->motors[2], 1000);
    ctx->motors[3]->vTable->MotorWrite(ctx->motors[3], 1000);

    /* Signal recovered during Stage 1 → back to ARMED immediately */
    if (!RCSignalLost(ctx->rcData)) {
        LogMessage(LOG_LEVEL_INFO, "RC SIGNAL RECOVERED\r\n");
        ctx->signalLostTick = 0;
        return ARMED;
    }

    /* Stage 1 expired → disarm */
    if ((HAL_GetTick() - ctx->signalLostTick) > FAILSAFE_STAGE1_MS) {
        ctx->signalLostTick = 0;
        DisarmESCs(ctx->motors);
        PidReset(ctx->rollPID);
        PidReset(ctx->pitchPID);
        PidReset(ctx->yawPID);
        return DISARMED;
    }

    return FAILSAFE;
}

armState_e HandleCrashDetected(smCtx_t *ctx) {
    LogMessage(LOG_LEVEL_ERROR, "CRASH DETECTED: DISARMING\r\n");
    DisarmESCs(ctx->motors);
    return DISARMED;
}
