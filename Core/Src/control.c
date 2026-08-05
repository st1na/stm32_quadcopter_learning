/*
 * control.c
 *
 *  Created on: Feb 27, 2026
 *      Author: st1na
 */

#include "control.h"

/*
 * Motor mixer for X-quad configuration
 * 
 * Motor layout:
 *   FR (0)   FL (1)
 *     \       /
 *      -------
 *      |     |
 *      -------
 *     /       \
 *   RR (3)   RL (2)
 *
 * Mixing equations for X-quad:
 * FR = throttle - roll - pitch + yaw
 * FL = throttle + roll - pitch - yaw
 * RL = throttle + roll + pitch + yaw
 * RR = throttle - roll + pitch - yaw
 */
void MotorMixerUpdate(float roll, float pitch, float yaw, float throttle,
                      motorDevice_t *motorDevice_FR,
                      motorDevice_t *motorDevice_FL,
                      motorDevice_t *motorDevice_RL,
                      motorDevice_t *motorDevice_RR) {
    
    /* Convert throttle from 0-1 range to -1 to 1 range for mixing */
    float throttle_norm = throttle * 2.0f - 1.0f;
    
    /* Apply X-quad motor mixing equation */
    float motor_norm[4];
    motor_norm[0] = throttle_norm - roll - pitch + yaw;  // FR (Front Right)
    motor_norm[1] = throttle_norm + roll - pitch - yaw;  // FL (Front Left)
    motor_norm[2] = throttle_norm + roll + pitch + yaw;  // RL (Rear Left)
    motor_norm[3] = throttle_norm - roll + pitch - yaw;  // RR (Rear Right)

    /* Clamp all motor outputs to [-1, 1] to prevent saturation */
    for (int i = 0; i < 4; i++) {
        motor_norm[i] = (motor_norm[i] < -1) ? -1 : (motor_norm[i] > 1) ? 1 : motor_norm[i];
    }

    /* Convert normalized [-1, 1] to PWM microseconds [1000, 2000]
     * Formula: pwm = motor_norm * 500 + 1500
     * Range: [-1, 1] * 500 + 1500 = [1000, 2000] microseconds
     */
    motorDevice_RR->vTable->MotorWrite(motorDevice_RR, (int)(motor_norm[3] * 500 + 1500));
    motorDevice_FR->vTable->MotorWrite(motorDevice_FR, (int)(motor_norm[0] * 500 + 1500));
    motorDevice_FL->vTable->MotorWrite(motorDevice_FL, (int)(motor_norm[1] * 500 + 1500));
    motorDevice_RL->vTable->MotorWrite(motorDevice_RL, (int)(motor_norm[2] * 500 + 1500));

    LogMessage(LOG_LEVEL_DEBUG, "PWM: FR=%d FL=%d RR=%d RL=%d\r\n",
        (int)(motor_norm[0] * 500 + 1500),
        (int)(motor_norm[1] * 500 + 1500),
        (int)(motor_norm[3] * 500 + 1500),
        (int)(motor_norm[2] * 500 + 1500));
}
