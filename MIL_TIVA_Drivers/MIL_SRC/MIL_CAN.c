/*
 * Name: MIL_CAN.c
 * Author: Marquez Jones
 * Date Modified: 3/23/2019
 * Desc: A set of wrapper functions I designed
 *       to standardized CAN use in the Lab
 *       this primarily is to reduce the number
 *       of bugs caused by the CAN bus
 *
 * Notes: CAN devices should be run at 100k bps
 *        and PCBs on the network should have on
 *        board termination resistors
 */

/* INCLUDES */
//includes
#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_can.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/can.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"

//MIL includes
#include"MIL_CAN.h"

/*
 * Desc: enables CAN which can be enabled on
 *       Ports B,E, or F for CAN0 
 * 		 or Port A for CAN1
 *
 * Notes: Does not enable interrupts
 *        or port clocks
 *        PORT and Interrupts
 *        must be enabled outside funciton
 *	
 *		  IF YOU SET CAN1_BASE AS YOUR BASE
 *   	  FUNCTION WILL DEFAULT TO CONFIGURING PORT A
 *
 * Hardware Notes:
 * CAN0:
 * PF0 - CANRX  PB4 - CANRX  PE4 - CANRX
 * PF3 - CANTX  PB5 - CANTX  PE5 - CANTX
 * CAN1:
 * PA0 - CANRX
 * PA1 - CANTX
 *
 * Inputs: 
 * port - from mil_port enum
 * base - CAN base(CAN0_BASE or CAN1_BASE) from tivaware
 * Assumes: Port clocks are enabled
 */
void MIL_InitCAN(mil_can_port_t port,uint32_t base){


    //pin configuration
    /*
     * Depending on the port, different pins
     * need to be configured
	 * 
	 * 
     */
    switch(port){
        case MIL_CAN_PORT_B:
            GPIOPinConfigure(GPIO_PB4_CAN0RX);
            GPIOPinConfigure(GPIO_PB5_CAN0TX);
            GPIOPinTypeCAN(GPIO_PORTB_BASE, GPIO_PIN_4 | GPIO_PIN_5);
            break;
        case MIL_CAN_PORT_E:
            GPIOPinConfigure(GPIO_PE4_CAN0RX);
            GPIOPinConfigure(GPIO_PE5_CAN0TX);
            GPIOPinTypeCAN(GPIO_PORTE_BASE, GPIO_PIN_4 | GPIO_PIN_5);
            break;
        case MIL_CAN_PORT_F:
            GPIOPinConfigure(GPIO_PF0_CAN0RX);
            GPIOPinConfigure(GPIO_PF3_CAN0TX);
            GPIOPinTypeCAN(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_3);
            break;
    }
	
	/*
	 * Enable can peripheral
	 * in case of CAN1 being selected, this section of code
	 * will also configure the CAN1 pins
	 */
	switch(base){
		case CAN0_BASE:
			SysCtlPeripheralEnable(SYSCTL_PERIPH_CAN0);
			break;
		case CAN1_BASE:
			SysCtlPeripheralEnable(SYSCTL_PERIPH_CAN1);
			GPIOPinConfigure(GPIO_PA0_CAN1RX);
			GPIOPinConfigure(GPIO_PA1_CAN1TX);
			GPIOPinTypeCAN(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
		
	}

		

    //Initialize CAN controller
    CANInit(base);

    //Set can retry to true
    CANRetrySet(base,1);

    //Set bit rates
    CANBitRateSet(base, SysCtlClockGet(), 100000);

    //enable CAN
    CANEnable(base);

}

/*
 * Desc: Enables interrupts on CAN0
 *
 * Notes: The Tiva CAN system has two
 *        separate interrupt sources.
 *        Either a controller error has
 *        occured(Tiva side) or there's
 *        a status change which can result
 *        from message transfer or a system
 *        bus error.
 *
 *        Further diagnostics in required in
 *        the external program
 *
 * Inputs: A pointer to your custom ISR
 * Assumes: Nothing
 */
void MIL_CANIntEnable(void (*func_ptr)(void),uint32_t base){

    //set a custom ISR
    CANIntRegister(base, func_ptr);

    //enable status interrupts
    //errors due to controller errors are ignored
    CANIntEnable(base, CAN_INT_MASTER | CAN_INT_STATUS);

	switch(base){
		case CAN0_BASE:
			IntEnable(INT_CAN0);
			break;
			
		case CAN1_BASE:
			IntEnable(INT_CAN1);
			break;
	}

}

/*
 * Desc: Enables the Port associated with the CAN system
 *
 * Note: For sake of your program making sense,
 *       only call this function if your CAN shares the
 *       port with no other GPIOs or peripherals
 *
 *       This just called the GPIO clock enable for
 *       the port
 */
void MIL_CANPortClkEnable(mil_can_port_t port){

    switch(port){

        case MIL_CAN_PORT_A:
            SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
            break;
        case MIL_CAN_PORT_B:
            SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
            break;
        case MIL_CAN_PORT_E:
            SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
            break;
        case MIL_CAN_PORT_F:
            SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
            break;

    }

}

/*
 * Desc: Easy to use function to transmit a message to the CAN bus
 * 	     This function declares a temporary Can message object and uses
 *	     object 0 to transmit the message
 * 
 * Inputs: 
 * canid - ID of your CAN node
 * pMsg  - pointer to your message 
 * MsgLen - the number of bytes in your message(up to 8 bytes)
 * base - which CAN module you want to use(CAN1_BASE or CAN0_BASE from tivaWare)
 */
void MIL_CANSimpleTX(uint32_t canid,uint8_t *pMsg,uint8_t MsgLen,uint32_t base){
	
	tCANMsgObject SimpleTXObj;
	SimpleTXObj.ui32MsgID = canid;
	SimpleTXObj.ui32Flags = 0;
	SimpleTXObj.ui32MsgLen = MsgLen;
	SimpleTXObj.pui8MsgData = pMsg;
	CANMessageSet(base, 0, &SimpleTXObj, MSG_OBJ_TYPE_TX);

}

/*
 * Desc: after configureing your mailbox struct
 *       pass it into this function in order to
 *       intialize reception
 *
 * Parameters:
 * pmailbox - a pointer to your mailbox
 */
void MIL_InitMailBox(MIL_CAN_MailBox_t *pmailbox){

    //basically copy mailbox parameters to TI CAN object
    pmailbox->msg_obj.ui32MsgID = pmailbox->canid;
    pmailbox->msg_obj.ui32MsgIDMask = pmailbox->filt_mask;

    //check for rx_flag
    if(pmailbox->rx_flag){
        pmailbox->msg_obj.ui32Flags = MSG_OBJ_RX_INT_ENABLE | MSG_OBJ_USE_ID_FILTER;
    }
    else{
        pmailbox->msg_obj.ui32Flags = MSG_OBJ_USE_ID_FILTER;
    }
    pmailbox->msg_obj.ui32MsgLen = pmailbox->msg_len;

    CANMessageSet(pmailbox->base, pmailbox->obj_num, &pmailbox->msg_obj, MSG_OBJ_TYPE_RX);
}

/*
 * Desc: if there is mail to be received
 *       this function will return that data
 *       to your buffer(pdata) this function
 *       will check if there is data but will
 *       not wait for new data
 *
 *       If there is data, it will return MIL_CAN_OK
 *       otherwise it will return MIL_CAN_NOK
 *
 * Parameters:
 * pmailbox - a pointer to your mailbox object struct
 * pdata - a pointer to your data buffer(an array)
 *
 * Returns:
 * mil_can_status_t - MIL_CAN_OK if there was new data
 *                    MIL_CAN_NOK if there is no data
 */
mil_can_status_t MIL_CAN_GetMail(uint8_t *pdata, MIL_CAN_MailBox_t *pmailbox){

    uint8_t ptemp[pmailbox->msg_len];
    pmailbox->msg_obj.pui8MsgData =  ptemp;

    if(CANStatusGet(pmailbox->base,CAN_STS_NEWDAT) & 0x01){

        for(uint8_t i = 0;i < pmailbox->msg_len;i++){

            pdata[i] = ptemp[i];

        }

        return MIL_CAN_OK;
    }

    else{

        return MIL_CAN_NOK;

    }

}
/*
 * Desc: Will return MIL_CAN_OK for new data
 *       or MIL_CAN_NOK if there is no data
 *
 * Paramters:
 * pmailbox - a pointer to your initialized mailbox
 *
 * Returns:
 * a mil_can_status_t depending on whether or not there's data
 */
mil_can_status_t MIL_CAN_CheckMail(MIL_CAN_MailBox_t *pmailbox){

    if(CANStatusGet(pmailbox->base,CAN_STS_NEWDAT) & 0x01){return MIL_CAN_OK;}
    else{return MIL_CAN_NOK;}

}







