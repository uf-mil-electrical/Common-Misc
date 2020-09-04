/*
 * Name: MIL_ADC.h
 * Author: Marquez Jones
 * Last Modified: 3/6/2019
 * Desc: Set of ADC wrapper functions to simplify ADC use
 *
 * Note VRef: This header defaults our reference voltage to the internal 3V source
 *            this means a 0xFFF raw output from the ADC corresponds to 3V
 *
 *            this is also hardcoded to single ended mode meaning we're capped at 0V
 *
 *            This can be represented by the equation of a line with the ADC value being our x
 *            and our output being y
 *
 *            so slope(m) = (3V - 0V)/(0xFFF - 0x000)
 *
 *            Voltage = m*(raw ADC)
 *
 * Note: In effect, the ADC module has a good amount of
 *       modularity that  I reduced with these functions
 *       to provide an easy to use set of functions
 *
 *       This should be sufficient for most ADC uses
 *       in MIL.
 *
 *       In the case it isn't, you'll have to read both
 *       the ADC sections from TivaWare and the Manual
 *       in order to make a more custom solution
 *
 *FREE LEARNING:
 *https://sites.google.com/site/luiselectronicprojects/tutorials/tiva-tutorials/tiva-adc/understanding-the-tiva-adc
 */

/*
 * WHAT YOU NEED TO UNDERSTAND WITHOUT READING THE ADC SECTION:
 * Refer to pages 800-801 of the TM4C123GH6PM manual
 * for tables to descibe sequences
 *
 * the Tiva has two possible ADC modules(ADC0 and ADC1)
 * Within these modules, you have multiple sequencers
 * effectively, what the sequencer does, is tell your adc
 * which channels it needs to check. So you can gather multiple
 * ADC samples from multiple different analog inputs and have those
 * results output to a buffer(some kind of array you declare)
 *
 * This is important to understand as it will change which sequencers you choose
 * There's 4 sequencers per ADC enumerated 0 to 3
 *
 * 3 can be assigned only one imput channel
 * 0 can be assigned 8 input channels
 *
 * Note if you were to write this code yourself, you check one pin
 * multiple times within a sequence. In this header, I hardcoded it to
 * assign unique channels to each sequence step
 *
 */

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "driverlib/adc.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"

#ifndef MIL_ADC_H_
#define MIL_ADC_H_

/*
 * Desc: bitmasks for all possible ADC pins
 *       this will be used by other functions
 *       in this header file
 *
 * TIVA ADC Notes:
 * The TM4C123G has 12 possible ADC input pins
 * The ports are verying
 * Please refer to table 23-3 for enumeration of pins
 *
 * Define Notes: On the right of each define
 *               I've indicated it's Pin name
 *               and the port and pin it's assoicated
 *               with. These are based on the TIVA
 *               semantics
 */
#define MIL_ADC_PIN0_bm  0x01 << 0  //AIN0, PE3
#define MIL_ADC_PIN1_bm  0x01 << 1  //AIN1, PE2
#define MIL_ADC_PIN2_bm  0x01 << 2  //AIN2, PE1
#define MIL_ADC_PIN3_bm  0x01 << 3  //AIN3, PE0
#define MIL_ADC_PIN4_bm  0x01 << 4  //AIN4, PD3
#define MIL_ADC_PIN5_bm  0x01 << 5  //AIN5, PD2
#define MIL_ADC_PIN6_bm  0x01 << 6  //AIN6, PD1
#define MIL_ADC_PIN7_bm  0x01 << 7  //AIN7, PD0
#define MIL_ADC_PIN8_bm  0x01 << 8  //AIN8, PE5
#define MIL_ADC_PIN9_bm  0x01 << 9  //AIN9, PE4
#define MIL_ADC_PIN10_bm 0x01 << 10//AIN10, PB4
#define MIL_ADC_PIN11_bm 0x01 << 11//AIN11, PB5

//literally the same defines as above but these
//are denoted by Port and Pin as well as channel
#define MIL_ADC_CH0_PE3_bm  0x01 << 0  //AIN0, PE3
#define MIL_ADC_CH1_PE2_bm  0x01 << 1  //AIN1, PE2
#define MIL_ADC_CH2_PE1_bm  0x01 << 2  //AIN2, PE1
#define MIL_ADC_CH3_PE0_bm  0x01 << 3  //AIN3, PE0
#define MIL_ADC_CH4_PD3_bm  0x01 << 4  //AIN4, PD3
#define MIL_ADC_CH5_PD2_bm  0x01 << 5  //AIN5, PD2
#define MIL_ADC_CH6_PD1_bm  0x01 << 6  //AIN6, PD1
#define MIL_ADC_CH7_PD0_bm  0x01 << 7  //AIN7, PD0
#define MIL_ADC_CH8_PE5_bm  0x01 << 8  //AIN8, PE5
#define MIL_ADC_CH9_PE4_bm  0x01 << 9  //AIN9, PE4
#define MIL_ADC_CH10_PB4_bm 0x01 << 10//AIN10, PB4
#define MIL_ADC_CH11_PB5_bm 0x01 << 11//AIN11, PB5

//pins associated with Port E
#define MIL_ADC_PORTE_gc  MIL_ADC_PIN0_bm | MIL_ADC_PIN1_bm | MIL_ADC_PIN2_bm | MIL_ADC_PIN3_bm | MIL_ADC_PIN8_bm | MIL_ADC_PIN9_bm
#define MIL_ADC_PORTD_gc  MIL_ADC_PIN4_bm | MIL_ADC_PIN5_bm | MIL_ADC_PIN6_bm | MIL_ADC_PIN7_bm
#define MIL_ADC_PORTB_gc  MIL_ADC_PIN10_bm | MIL_ADC_PIN11_bm

/*
 * Refer to pages 800-801 of the TM4C123GH6PM manual
 * for tables to descibe sequences
 *
 * From an abstract perspect, each ADC module(ADC0 and ADC1)
 * have 4 sequencers attached to them. The sequencers are
 * what actually d
 *
 */

//these defines are arbitrary as you can simply pass in the
//the raw integer value,but these will increase readability
#define MIL_ADC_SEQ0 0x00
#define MIL_ADC_SEQ1 0x01
#define MIL_ADC_SEQ2 0x02
#define MIL_ADC_SEQ3 0x03

/*
 * For the purpose of abstraction
 * I will limit our possible ADC
 * trigger sources to 3 options
 *
 * NOTE: VIEW ASSOCIATED FUNCTION IN TIVAWARE MANUAL
 * MIL_ADC_SoftTrig - trigger in software using ADCProcessorTrigger();
 * MIL_ADC_TimTrig  - triggered by configuring timer with TimerControlTrigger();
 * MIL_ADC_AlwaysTrig - ADC will always trigger
 *
 * These will be used by my ADC init function
 */
typedef enum{
    MIL_ADC_SoftTrig,
    MIL_ADC_TimTrig,
    MIL_ADC_AlwaysTrig
}mil_trig_t;

//
/*
 * If there is a problem,
 * return NOK(not OK)
 *
 * else return ADC_OK
 */
typedef enum{

    MIL_ADC_OK,
    MIL_ADC_NOK

}mil_adc_stat_t;

/*
 * Desc: This function will configure the selected ADC channel
 *       as enumerated by our MIL_ADC_PINx_bm defines. Each of
 *       of those is associated with one the 12 ADC channels
 *
 * Note: This function effectively calls the
 *       GPIOPinTypeADC() function for each ADC pin
 *       function will also call the peripheral clock enables
 *
 * Parameters: This function takes in a bitfield
 *             of each ADC pin desired bit wised ORed
 *             together
 *
 * Para Note: So if you wanted pins 3 and 7, you'd pass in
 *            uint16_t field = MIL_ADC_PIN3_bm | MIL_ADC_PIN7_bm
 */
void MIL_ADCPinConfig(uint16_t bitfield);

/*
 * Desc:This function enables individual sequences
 *      You will need to call this for each sequence
 *      you sequence you need to call this function
 *
 * Note: Each ADC has multiple sequences that control the
 *       ADC
 *
 *       This also hardcodes for single ended mode. This
 *       If you need differential, either consult the TivaWare
 *       manual and modify this function,write your own function,
 *       or contact someone who feels like modifying it for you
 *
 * Sequence Note: know that which sequence you select will limit how
 *                how many channels/pins you can assign.
 *
 *                If you input more pins than are available to that sequence
 *                the function will disregard those pins
 *
 *                Sequence 0 - 1 step/channel
 *                Sequence 1 - 4 steps/channels
 *                Sequence 2 - 4 steps/channels
 *                Sequence 3 - 8 steps/channels
 *
 * Interrupts Note: This function will always configure the
 *              ADC to set it's ISR flag without enabling the
 *              interrupt.
 *
 *              If you want to use this without interrupts,
 *              poll the ISR flag, and clear the interrupt using
 *              IntClear
 *
 *              refer to page 32 section 4.2.2.18
 *              for more information on using
 *              the interrupts
 *
 *              You'll need to register an interrupt
 *              function and call the enable functions
 *
 * Parameters:
 * adc_base - from TIVA library either ADC0_BASE or ADC1_BASE
 * Seq_num - which sequence you wish to enable(see Sequence defines in this header)
 * pin_bitfield - what pins you're using(see pin defines in this header)
 * trig - from mil_trig_t, this specifies what triggers your ADC
 *
 *
 * USE NOTES TO GET DATA:
 *  Poll for interrupt flag using
 *  ADCIntStatus(ADCx_BASE, MIL_ADC_SEQx, false);
 *
 *  Clear the Interrupt flag with
 *  ADCIntClear(ADCx_BASE, MIL_ADC_SEQx);
 *
 * receive data into buffer with
 * ADCSequenceDataGet(ADC0_BASE,MIL_ADC_SEQx,array_pointer); where the array point points to an array of 32 bit values
 *
 * Arrays are points in C/C++
 *
 * Returns: mil_adc_stat_t
 *          the only section of code that's able to return this is assigning
 *          sequences(check source code)
 *
 *          It basically means you passed in a bad parameter that caused the function
 *          not to exit relating to the bitfield
 */
mil_adc_stat_t MIL_ADCSeqInit(uint32_t base,
                    uint8_t seq_num,
                    uint16_t pin_bitfield,
                    mil_trig_t trig);

/*
 * Desc: enable adc interrupts
 *
 * Note: will trigger upon a conversion finishing
 *
 * Parameters:
 *  isr_ptr - pointer to your isr
 *  base - ADC base
 *  seq_num- your sequence number
 *
 */
void MIL_ADCIntEnable(void (*isr_ptr)(void),
                      uint32_t base,
                      uint8_t seq_num);

/*
 * Desc: Runs get data sequence specified in the ADCSeqInit function
 *       This function can also be called in interrupts if need be
 *
 * Note:
 *      This includes a wait function that will wait for new data or
 *      the user specified timer is reached
 *
 *      You can also implment there to be no wait, in which case the function
 *      will exit immediately if there is no new data
 *
 * Sequence:
 *  Poll for interrupt flag using
 *  ADCIntStatus(ADCx_BASE, MIL_ADC_SEQx, false);
 *
 *  Clear the Interrupt flag with
 *  ADCIntClear(ADCx_BASE, MIL_ADC_SEQx);
 *
 * receive data into buffer with
 * ADCSequenceDataGet(ADC0_BASE,MIL_ADC_SEQx,array_pointer); where the array point points to an array of 32 bit values
 *
 * Parameters:
 *  base - from TivaWare, your ADCx_BASE(either 0 or 1 for or x)
 *  seq_num- from mil header, which MIL_ADC_SEQx( 0 to 3 for x)
 *  timeout- how many cycles you wish to wait for the ADC to provide data
 *  pbuffer - pointer to output buffer (MUST USE POINT TO UIN32_T OR IT'LL DIE)
 *
 * Returns:
 *  mil_adc_status_t - MIL_ADC_OK if there's new data
 *                     MIL_ADC_NOK if there's no new data
 */
mil_adc_stat_t MIL_ADCGetData(uint32_t base,
                              uint8_t seq_num,
                              uint32_t timeout,
                              uint32_t *pbuffer);

/*
 * Desc: This function will convert a raw single ended ADC value to
 *       its double equivalent
 *
 * Note: this is based on the equation of a line
 *       in effect, the max adc value is 0xFFF which corresponds to
 *       your reference voltage.
 *
 *       implemented with equation
 *
 *       m = 3V/0xFFF
 *
 *       Voltage = m*(raw ADC)
 */
float MIL_ADC_HextoFloat(uint16_t adc_value);

/*
 * Desc: just incase you needed to convert the ADC values to ascii
 *       This function will take your 12 bit ADC value and output
 *       a string including its raw decimal and hex values
 *
 * Note: output string is 3 bytes long
 *
 * Note: I used this for debugging
 */
void MIL_ADC_HEXtoASCII(uint16_t adc_value,char *pstring);

/*
 * Desc: Converts a raw adc value into an ascii string
 *
 * Note: outstring is 4 bytes long
 *
 * Parameters:
 *  adc_value - from the ADC output
 *  pstring- pointer to outstring
 *
 */
void MIL_ADC_FloattoASCII(uint16_t adc_value,char *pstring);

#endif /* MIL_ADC_H_ */
