/*
 * Name: MIL_BR_ESC.c
 * Author: Marquez Jones
 * Desc: Useful tools for interfacing
 *       PWM with Blue Robotics Basic
 *       ESCs to control the T200 Thrusters
 *
 *       Also some non-hardware specific
 *       functions for mapping Video Ray
 *       input to Blue robotics thrusters
 *
 * Notes: View this link for details on BR Basic ESC
 * https://www.bluerobotics.com/store/thrusters/speed-controllers/besc30-r3/
 */
#include "MIL_BR_ESC.h"
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/pwm.h"
#include "driverlib/sysctl.h"

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
 *
 * EQUATION: duty = (0.4 * thrust + 1.5)/BR_ESC_PERIOD_MS
 * Input: VR_Thrust value
 * Output: The corresponding duty cycle
 */
float MIL_BR_linear_duty(float Thrust){

    //check if thrust is 0
    if(Thrust){

        //-1 corresponds to 1100us/2000us duty, 1 corresponds to 1900us/2000us duty cycle
        return (Thrust * 0.4 + 1.5)/BR_ESC_PERIOD_MS;

    }
    /*
     * 0 thrust implies halt
     * which implies a 75% duty cycle for
     * the PWM signal
     */
    else{
        return BR_STOP_THRUST_DUTY;
    }



}

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
uint32_t MIL_BR_linear_per(float Thrust, uint32_t ui32Base, uint32_t ui32Gen){

    return MIL_BR_linear_duty(Thrust) * PWMGenPeriodGet(ui32Base, ui32Gen);

}
