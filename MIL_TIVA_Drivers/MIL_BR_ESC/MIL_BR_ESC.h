/*
 * Name: MIL_BR_ESC.h
 * Author: Marquez Jones
 * Desc: Useful tools for interfacing
 *       PWM with Blue Robotics(BR) Basic
 *       ESCs to control the T200 Thrusters
 *
 *       Also some non-hardware specific
 *       functions for mapping Video Ray
 *       thrust to Blue robotics PWM signal
 *
 * Notes: View this link for details on BR Basic ESC
 * https://www.bluerobotics.com/store/thrusters/speed-controllers/besc30-r3/
 */

/* INCLUDES */
#include <stdbool.h>
#include <stdint.h>
#include "driverlib/pwm.h"


#ifndef MIL_BR_ESC_H_
#define MIL_BR_ESC_H_

//the ESC expects pwm signal to have a period of 2ms
#define BR_ESC_PERIOD_MS 2
//In seconds
#define BR_ESC_PERIOD_SEC 0.002

//useful duty cycles
/*
 * Our application processor effectively
 * sends commands to the ESC via PWM signals
 * which the ESC then uses to control the speed
 * of the thrusters
 */
#define BR_MAX_FWD_THRUST_DUTY 1.9/BR_ESC_PERIOD_MS //max forward
#define BR_MAX_REV_TRHUST_DUTY 1.1/BR_ESC_PERIOD_MS //max reverse
#define BR_STOP_THRUST_DUTY 1.5/BR_ESC_PERIOD_MS //full stop

/*Useful macros */
/*
 * Desc:
 * All PWM periods passed in will have to be some
 * sort of percent of the total period
 *
 * These macros automatically call the useful functions
 *
 * Note:
 * If you are not familiar with C macros
 * they effectively work like defines for
 * functions so when the code is compiled
 * these function calls will be loaded in place of the
 * macros
 *
 * macros are not in themselVes function calls but "wrappers"
 * for functions
 *
 * EQUATION: duty = (0.4 * thrust + 1.5)/BR_ESC_PERIOD_MS
 *
 * Inputs:
 * base - PWM base from tivaware
 * gen - PWM generator from tivaware
 *
 */
#define PWM_STOP_PER(base,gen) PWMGenPeriodGet(base, gen) * BR_STOP_THRUST_DUTY
#define PWM_MAX_FWD_PER(base,gen) PWMGenPeriodGet(base, gen)* BR_MAX_FWD_THRUST_DUTY
#define PWM_MAX_REV_PER(base,gen) PWMGenPeriodGet(base, gen)* BR_MAX_REV_THRUST_DUTY


/*
 * Desc: This function maps a float value 1 to -1
 *       to the corresponding duty cycle needed
 *       the blue robotics command
 *
 *       1 being max forward thrust and -1 being max
 *       reverse thrust
 *
 *       The video ray protocol sends a float whose
 *       value ranges from 1 to -1 which is then
 *       interpreted by the video ray thrusters
 *
 *       If your firmware solution emulates the
 *       video ray protocol, you can use this function
 *       to map the thrust value to a duty cycle to be applied
 *       to the blue robotics PWM signal
 * Input: Thrust value (1 to -1)
 * Output: The corresponding duty cycle
 */
float  MIL_BR_linear_duty(float Thrust);

/*
 * Desc: This function maps a float value 1 to -1
 *       to the corresponding period needed
 *       the blue robotics command
 *
 *       1 being max forward thrust and -1 being max
 *       reverse thrust
 *
 * NOTE: this is very similar to MIL_BR_linear_duty
 *       but this function will output a period value
 *       to be passed into the Tivaware pulse width set
 *       function
 *
 * Input:
 *      Thrust - value (1 to -1)
 *      ui32Base - PWM base from TivaWare
 *      ui32Gen - which PWM generator you're using
 * Output: The corresponding period
 */
uint32_t MIL_BR_linear_per(float Thrust, uint32_t ui32Base, uint32_t ui32Gen);

#endif /* MIL_BR_ESC_H_ */





