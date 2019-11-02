/*
 * Name: MIL_CAN.c
 * Author: Marquez Jones0
 * Date Modified: 11/2/2019
 * Desc: A set of wrapper functions I designed
 *       to standardized CAN use in the Lab
 *       this primarily is to reduce the number
 *       of bugs caused by the CAN bus
 *
 * Notes: CAN devices should be run at 200k bps
 *        and PCBs on the network should have on
 *        board termination resistors
 * BAUD UPDATE: CAN INIT NOW HAS CONFIGURABLE BAUD RATE
 */

#include "driverlib/can.h"

#ifndef MIL_CAN_H_
#define MIL_CAN_H_

/*
 *Desc: Port selection will come from this enum
 */
typedef enum {
    MIL_CAN_PORT_A,
    MIL_CAN_PORT_B,
	MIL_CAN_PORT_C,
	MIL_CAN_PORT_D,
    MIL_CAN_PORT_E,
    MIL_CAN_PORT_F
}mil_can_port_t;

/*
 *Desc: status flags
 */
typedef enum {
   MIL_CAN_NOK, //operation failed
   MIL_CAN_OK   //operation succeeded
}mil_can_status_t;

/*
 * Desc: Contains mailbox attributes
 *       a mailbox is simply where you
 *       will receive data from the CAN bus
 *
 * WHAT YOU NEED TO UNDERSTAND ABOUT CAN:
 * In effect every ECE is sharing the same
 * communication line, in this every ECE can
 * "hear" all messages. With this, your device
 * may not care about every single message on the
 * bus. Given this, you can choose to filter for
 * certain message IDs.
 *
 * NOTE:
 * This is an alternate struct to use for
 * configuring CAN message objects
 *
 * This struct is actually slightly more
 * complex than the TI one, but I believe is
 * more organized. I also believe it'll make
 * function use more streamlined. You can choose
 * either method ,but this is what I created to
 * interface with other reception functions I made
 *
 * This just encompasses more relevant CAN paramters
 * like the object number
 *
 * PARAMETERS NOTE:
 * FOR USE IN MIL_CAN FUNCTIONS, COMPLETELY
 * IGNORE THE msg_obj PARAMETER. IT'S CONFIGURED
 * FOR YOU IN OTHER MIL_CAN FUNCTIONS
 *
 * PARAMETERS:
 * canid - the target ID you wish to filter for
 * filt_mask - which bits matter in the canid
 * base - TIVA CANx_BASE from tivaware
 * msg_len - how long the expected CAN data is
 * obj_num - each unique mailbox needs a unique number
 * rx_flag - only set this variable if you intend on setting up
 *               CAN interrupts otherwise make it 0
 * buffer  - a pointer to your out data (MUST BE SET)
 *
 * OBJ_NUM NOTE: CAN OBJECTS ARE ENUMERATED 1 TO 32 NOT 0 TO 31
 *
 */
typedef struct{

  uint32_t canid;          //ALL IDs should be 8 bits long
  uint32_t filt_mask;      //bit mask
  uint32_t base;           //TI CANx_BASE value
  uint8_t  msg_len;         //values 1 to 8
  uint8_t  obj_num;         //values 1 to 32
  uint8_t  rx_flag_int;         //value 0 or 1
  uint8_t *buffer;           //pointer to your out array
  tCANMsgObject msg_obj;    //used to interface with other TI functions(you do not configure this)

} MIL_CAN_MailBox_t;

/*
 * Desc: enables CAN0 which can be enabled on
 *       Ports B,E, or F
 *
 * Notes: Does not enable interrupts
 *        or port clocks
 *        PORT and Interrupts
 *        must be enabled outside funciton
 *
 * Hardware Notes:
 * CAN0:
 * PF0 - CANRX  PB4 - CANRX  PE4 - CANRX
 * PF3 - CANTX  PB5 - CANTX  PE5 - CANTX
 * CAN1:
 * PA0 - CANRX
 * PA1 - CANTX
 *
 * Inputs: port from mil_port enum
 * Assumes: Port clocks are enabled
 */
void MIL_InitCAN(mil_can_port_t port,uint32_t base,uint32_t bps);

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
void MIL_CANIntEnable(void (*func_ptr)(void),uint32_t base);


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
void MIL_CANPortClkEnable(mil_can_port_t port);


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
void MIL_CANSimpleTX(uint32_t canid,uint8_t *pMsg,uint8_t MsgLen, uint32_t base);


/*
 * Desc: after configureing your mailbox struct
 *       pass it into this function in order to
 *       intialize reception
 *
 * Parameters:
 * pmailbox - a pointer to your mailbox
 */
void MIL_InitMailBox(MIL_CAN_MailBox_t *pmailbox);

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
mil_can_status_t MIL_CAN_GetMail(MIL_CAN_MailBox_t *pmailbox);

/*
 * Desc: Will return MIL_CAN_OK for new data
 *       or MIL_CAN_NOK if there is no data
 *
 * Paramters:
 * pmailbox - a pointer to your initialized mailbox
 *
 * Returns:
 * a mil_can_status_t depending on whether or not there's data
 *
 * NOTE - DATA WILL BE RETURNED TO YOUR OUT BUFFER
 */
mil_can_status_t MIL_CAN_CheckMail(MIL_CAN_MailBox_t *pmailbox);

#endif /* MIL_CAN_H_ */
