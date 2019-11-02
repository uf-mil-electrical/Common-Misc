/*
 * Name: Blue_Robotics_Test
 * Author: Marquez Jones
 * Desc: Test program to prototype control
 *       of BR thrusters
 */

//includes
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/pwm.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"

#include "MIL_BR_ESC.h" //ESC header
#include "MIL_CLK.h"

//defines
#define MS1 0.001 * 16000000 //cycles for 1ms
#define SEC1 16000000        //cycles for 1sec
#define PUSH_SW_1 GPIO_PIN_4 //SW 1

/***********************************************MACROS*******************************************/
/*
 * This demo uses PWM1 on GEN3
 * I will make a more universal version of this function later
 */
#define PWM1_STOP_PER() PWMGenPeriodGet(PWM1_BASE, PWM_GEN_3) * BR_STOP_THRUST_DUTY
#define PWM1_MAX_FWD_PER() PWMGenPeriodGet(PWM1_BASE, PWM_GEN_3)* BR_MAX_FWD_THRUST_DUTY
#define PWM1_MAX_REV_PER() PWMGenPeriodGet(PWM1_BASE, PWM_GEN_3)* BR_MAX_REV_TRHUST_DUTY

/*
 * Delay macros
 */
#define SEC_1_DELAY()  SysCtlDelay((SEC1/3))
#define SEC_15_DELAY() SysCtlDelay(15*(SEC1/3))
#define SEC_30_DELAY()  SysCtlDelay(30*(SEC1/3))

/***********************************************FUNC PROTOTYPES*******************************************/

/*
 * Desc: Configure PWM1 Gen3 ,Bits 6 and 7
 *       Duty cycle and signal output enable
 *       will be handled externally
 *
 *       Blue robotics ESC expects a 2ms period
 *
 * Hardware Notes:
 * M1PWM6 : PF2
 * M1PWM7 : PF3
 *
 */
void Init_PWM1_Gen3_67(void);

/*
 * Desc: Test Sequence as described by Alan
 * 75% forward for 30 seconds,
 * 100% forward 15 seconds,
 * 75% forward 30 seconds,
 * 25% forward 15 seconds,
 * 25% reverse 15 seconds,
 * 75% reverse 30 seconds,
 * 100% reverse 15 seconds
 *
 * ASSUMES: ASSUMES Init_PWM1_Gen3_67 has been called
 */
void PWM_Test_Sequence(void);
//same as above but implmented with a for loop
void PWM_Test_Loop(void);

/*
 * Desc: Sets the pwm period for both PWM bits 6 and 7 on Gen 3
 * Inputs:
 * width - based on the (high period/total period)
 *
 * Notes: This is not the duty cycle, this is a time measurement
 */
void PWM_Test_SetWidth(uint32_t width);

/*
 * Initialize switch 1 on launchpad  as input GPIO with pull up resistor
 */
void Init_SW1(void);

/********************************************GLOBAL VARIABLES**********************************/

uint8_t sw_flag = 0x00;

/*********************************************ISR PROTOTYPES**********************************/

void PortF_SW_ISR(void);

/*********************************************MAIN*************************************************/

int main(void){

    MIL_ClkSetInt_16MHz();

    //initialize the switch
    Init_SW1();

    //Initialize PWM
    Init_PWM1_Gen3_67();

    //master int enable
    IntMasterEnable();

    /*
     * Initiailize Thruster
     *
     * For blue robotics, a stop command must be issued
     * to the thruster before forward or reverse signals
     * This initializes the thruster
     *
     * (pulse width) = (duty cycle) * (PWM period)
     */
    PWMPulseWidthSet(PWM1_BASE,
                     PWM_OUT_6,
                     PWM1_STOP_PER());

    PWMPulseWidthSet(PWM1_BASE,
                     PWM_OUT_7,
                     PWM1_STOP_PER());

    /* set both pins to outputs */
    PWMOutputState(PWM1_BASE,
                   PWM_OUT_6_BIT |
                   PWM_OUT_7_BIT,
                   true);

    while(1){

        //wait for 1 to be pressed
        if(sw_flag){

            //beging test sequence as specified by Alan3
            PWM_Test_Loop();

        }
        else{

            //stop thrusters
            PWMPulseWidthSet(PWM1_BASE,
                                PWM_OUT_6,
                                PWM1_STOP_PER());

            PWMPulseWidthSet(PWM1_BASE,
                                PWM_OUT_7,
                                PWM1_STOP_PER());

        }

    }

}

/*********************************************FUNC DEFINITIONS**********************************/

/*
 * Desc: Configure PWM1 Gen3 ,Bits 6 and 7
 *       Duty cycle and signal output enable
 *       will be handled externally
 *
 *       Blue robotics ESC expects a 2ms period
 *
 * Hardware Notes:
 * M1PWM6 : PF2
 * M1PWM7 : PF3
 *
 */
void Init_PWM1_Gen3_67(void){

        //PWM clock enable
        SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);

//        //port F clock enable
//        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

        //enable PWM functions on PF2 and PF3
        GPIOPinConfigure(GPIO_PF2_M1PWM6);
        GPIOPinConfigure(GPIO_PF3_M1PWM7);

        //Configure PF2 and PF3 as PWM
        GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_2 | GPIO_PIN_3);

        /*
         * Configure:
         * PWM Mod 1
         * Generator 3
         * Up down mode
         * No sync(you can sync generators together)
         */
        PWMGenConfigure(PWM1_BASE,
                        PWM_GEN_3,
                        PWM_GEN_MODE_UP_DOWN |
                        PWM_GEN_MODE_NO_SYNC);

        /* (desired period in seconds) * (clock frequency) = PWM Period */
        PWMGenPeriodSet(PWM1_BASE, PWM_GEN_3, BR_ESC_PERIOD_SEC * (SysCtlClockGet()));

        //enable pwm
        PWMGenEnable(PWM1_BASE, PWM_GEN_3);



}

/*
 * Desc: Test Sequence as described by Alan
 * 75% forward for 30 seconds,
 * 100% forward 15 seconds,
 * 75% forward 30 seconds,
 * 25% forward 15 seconds,
 * 25% reverse 15 seconds,
 * 75% reverse 30 seconds,
 * 100% reverse 15 seconds
 *
 * ASSUMES: ASSUMES Init_PWM1_Gen3_67 has been called
 *
 * NOTE: MIL_BR_linear_per function will generate
 *       a period based on the input duty cycle as a
 *       float
 */
void PWM_Test_Sequence(void){

        /*
         * This sets the thruster go to full forward at max thrust
         */
        PWM_Test_SetWidth(PWM1_MAX_FWD_PER());

        //15 second delay
        SEC_15_DELAY();

        /* 75% forward */
        PWM_Test_SetWidth(MIL_BR_linear_per(0.75,PWM1_BASE, PWM_GEN_3));

        //30 second delay
        SEC_30_DELAY();

        /* 25% forward */
        PWM_Test_SetWidth(MIL_BR_linear_per(0.25,PWM1_BASE, PWM_GEN_3));

        //15 second delay
        SEC_15_DELAY();

        /* 25% reverse */
        PWM_Test_SetWidth(MIL_BR_linear_per(-0.25,PWM1_BASE, PWM_GEN_3));

        //15 second delay
        SEC_15_DELAY();

        /* 50% reverse*/
        PWM_Test_SetWidth(MIL_BR_linear_per(-0.5,PWM1_BASE, PWM_GEN_3));

        //15 second delay
        SEC_15_DELAY();

        /* 75% reverse */
        PWM_Test_SetWidth(MIL_BR_linear_per(-0.75,PWM1_BASE, PWM_GEN_3));

        //30 second delay
        SEC_30_DELAY();

        /* full reverse */
        PWM_Test_SetWidth(PWM1_MAX_REV_PER());

        //15 second delay
        SEC_15_DELAY();

        PWM_Test_SetWidth(PWM1_STOP_PER());

        //15 second delay
        SEC_15_DELAY();
}


//steps by 25%
void PWM_Test_Loop(){

    const float step = 0.125;
    float duty = 0;

    while(duty <= 4*step){

        duty += step;

        PWM_Test_SetWidth(MIL_BR_linear_per(duty,PWM1_BASE, PWM_GEN_3));


        if(abs(duty) == 3*step){

            //30 second delay
            SEC_30_DELAY();

        }
        else{

            SEC_15_DELAY();

        }
    }

    //step down from 100% to -100%
    while(duty >= -4*step){

          duty -= step;

          PWM_Test_SetWidth(MIL_BR_linear_per(duty,PWM1_BASE, PWM_GEN_3));

          if(abs(duty) == 3*step){

              //30 second delay
              SEC_30_DELAY();

          }
          else{

              SEC_15_DELAY();

          }

    }
    while(duty <= 2*step){

        duty += step;

        PWM_Test_SetWidth(MIL_BR_linear_per(duty,PWM1_BASE, PWM_GEN_3));

        if(abs(duty) == -3*step){

            //30 second delay
            SEC_30_DELAY();

         }
         else{

             SEC_15_DELAY();

         }

    }
    PWM_Test_SetWidth(PWM1_STOP_PER());


}

/*
 * Initialize switch 1 on launchpad  as input GPIO with pull up resistor
 */
void Init_SW1(void){

    //enable peripheral clock
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    //wait for peripheral to be ready
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));

    //set these pin for switch
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, PUSH_SW_1);

    /*
     * Configure the push input to have an interal pull up resistor
     *
     * Notes:the gpio_strength parameter would set the current output to be
     * 2mA if the pin were configured as an output. In this case it has no effect
     * since the pin is an input pin
     */
    GPIOPadConfigSet(GPIO_PORTF_BASE,
                     PUSH_SW_1,
                     GPIO_STRENGTH_2MA,
                     GPIO_PIN_TYPE_STD_WPU);

    /*Configure interrupt*/
    //set ISR function
    GPIOIntRegister(GPIO_PORTF_BASE, &PortF_SW_ISR);

    //configure pin for falling edge
    GPIOIntTypeSet(GPIO_PORTF_BASE,  PUSH_SW_1 , GPIO_FALLING_EDGE);

    //enable interrupts on for that pin
    GPIOIntEnable(GPIO_PORTF_BASE, PUSH_SW_1);

}

/*
 * Desc: Sets the pwm period for both PWM bits 6 and 7 on Gen 3
 * Inputs:
 * width - based on the (high period/total period)
 *
 * Notes: This is not the duty cycle, this is a time measurement
 */
void PWM_Test_SetWidth(uint32_t width){

    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6,
                     width);

    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7,
                     width);

}


/*********************************************ISR DEFINITIONS**********************************/

void PortF_SW_ISR(void){

    //GPIOPinWrite(GPIO_PORTF_BASE, BLUE_LED, 0xFF);
    GPIOIntClear(GPIO_PORTF_BASE,PUSH_SW_1);

    sw_flag ^= 0xFF;

}







