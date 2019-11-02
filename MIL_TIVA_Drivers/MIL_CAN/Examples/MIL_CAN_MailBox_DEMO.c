/*
 * Name: MIL_CAN Mailbox example
 * Author: Marquez Jones
 * Date Created: 4/21/2019
 * Desc: Full demonstration of receiving and transmitting messages using MIL_CAN
 *
 * HARDWARE NOTES:
 * CAN_TX - PA1
 * CAN_RX - PA0
 *
 *DEMO NOTES:
 *To actually Demo the functionality of this code, you will need two
 *separate boards,one sending and one receiving
 *
 * WHAT YOU NEED TO KNOW:
 * I HIGHLY RECOMMEND YOU GET AN OVERVIEW OF HOW CAN WORKS IN
 * THE ABSTRACT BEFORE LOOKING AT THIS CODE
 *
 */

//includes
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/timer.h"
#include "driverlib/pwm.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"

#include "MIL_CLK.h"

/*
 * MAKE SURE YOU ARE USING THE 4/20/2019 MIL_CAN HEADER
 * PREVIOUS VERSION DO NOT HAVE MAILBOX FUNCTIONALITY
 */
#include "MIL_CAN.h"

/********DEFINES START******/
#define CANID_RX        0x37   //ID we wish to filter for,task group = 3, ECU = 7
#define CANID_TX        0x38   //our ECUs ID will will transmit with, task group = 3, ECU = 7
#define CAN_MSG_LEN_8    0x8    //CAN message lengths


/********DEFINES END********/


/*********************************************MAIN*************************************************/

int main(void){

    MIL_ClkSetInt_16MHz();

    /**************************************CAN INIT START********************/

    /*enable port A peripheral clock*/
    /*
     * This function is unnecessary if you already
     * enable port A somewhere else in your code
     *
     * You'll notice in other MIL files, I don't
     * even include a function like this since ports
     * tend to be shared by multiple peripherals
     */
    MIL_CANPortClkEnable(MIL_CAN_PORT_A);

    /*Initialize CAN module*/
    /*
     * This is how your initialize the TIVA's CAN hardware.
     * AFter initializing this, you will be able to send and
     * transmit messages.
     *
     * Note: CAN0 can use multiple Ports where as CAN1 is restricted
     * to port A due to constraints originating from the MCU.
     */
    MIL_InitCAN(MIL_CAN_PORT_A,CAN1_BASE);

    /*CONFIGURE YOUR RX MAILBOX*/

    //for our demo will will receive 8 byte long messages
    uint8_t CAN_buffer[CAN_MSG_LEN_8];

    //declare mailbox object
    MIL_CAN_MailBox_t MailBox;

    /*configure mailbox parameters*/
    /*
     * Note: the only parameter in the struct you should not configure is
     *       the tCANMsgObject(msg_obj). This is configured in the mailbox
     *       init function and will get overwritten regardless
     */
    MailBox.canid = CANID_RX;   //INSERT THE ID YOU WANT TO FILTER FOR

    /*
     * For sake of demo purposes, the filter mask is set to all 0s
     * meaning we'll accept any id basically
     *
     * More on this, you can configure the CAN module to only check certain
     * bits in the ID. Setting a bit in the filter mask, means that bit matters
     * as far as filtering. Setting a bit as 0 ,means that bit is a don't care
     *
     * NOTE: YOUR CAN MODULE WILL NOT RECEIVE MESSAGES FROM IDS, THAT DON'T
     *       FIT YOUR FILTER PARAMETERS.
     *
     *       IF YOU SET THE FILT MASK TO 0xFF, this mailbox would not receive
     *       messages from IDS other than our CANID_RX.
     *
     */
    MailBox.filt_mask = 0x00;   //set filt mask to don't care
    MailBox.base = CAN1_BASE;   //which can base are you using?, here we use CAN module 1
    MailBox.msg_len = CAN_MSG_LEN_8; //set message length to 8

    /*
     * There's a finite number of CAN message objects we can use for the TIVA,32 to be exact
     * numbering 1 to 32(yes the first object is 1 and not 0)
     */
    MailBox.obj_num = 1; //this mailbox will use message object 1 in the CAN module
    MailBox.rx_flag_int = 0; //if you wanted to use RX interrupts, set this flag

    /*
     * You must allocate memory for CAN messages
     * Note,CAN messages will be placed here when running
     * CAN_GetMail
     */
    MailBox.buffer = CAN_buffer;

    /*Initialize reception mailbox*/
    MIL_InitMailBox(&MailBox);

    /**************************************CAN INIT END********************/


    while(1){

       //If a message has been placed in your mailbox
       if(MIL_CAN_CheckMail(&MailBox) == MIL_CAN_OK){

           //note, messages will be output to designated buffer
           //code may change in the future for you to designate a separate buffer here
           MIL_CAN_GetMail(&MailBox);

           //echo back that message using our ECUs TX ID
           MIL_CANSimpleTX(CANID_TX,CAN_buffer,CAN_MSG_LEN_8,CAN1_BASE);

       }

    }

}


