/*
 * Name: MIL_ADC.h
 * Author: Marquez Jones
 * Last Modified: 3/6/2019
 * Desc: Set of ADC wrapper functions to simplify ADC use
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

//MIL includes
#include "MIL_ADC.h"

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
void MIL_ADCPinConfig(uint16_t bitfield){

    if(bitfield & MIL_ADC_PORTB_gc){

        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    }
    if(bitfield & MIL_ADC_PORTD_gc){

        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);

    }
    if(bitfield & MIL_ADC_PORTE_gc){

        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

    }
    if(bitfield & MIL_ADC_PIN0_bm){

        GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3);

    }
    if(bitfield & MIL_ADC_PIN1_bm){

        GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_2);

    }
    if(bitfield & MIL_ADC_PIN2_bm){

        GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_1);

    }
    if(bitfield & MIL_ADC_PIN3_bm){

        GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_0);

    }
    if(bitfield & MIL_ADC_PIN4_bm){

        GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_3);

    }
    if(bitfield & MIL_ADC_PIN5_bm){

        GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_2);

    }
    if(bitfield & MIL_ADC_PIN6_bm){

        GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_1);

    }
    if(bitfield & MIL_ADC_PIN7_bm){

        GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_0);

    }
    if(bitfield & MIL_ADC_PIN8_bm){

        GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_5);

    }
    if(bitfield & MIL_ADC_PIN9_bm){

        GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_4);

    }
    if(bitfield & MIL_ADC_PIN10_bm){

        GPIOPinTypeADC(GPIO_PORTB_BASE, GPIO_PIN_4);

    }
    if(bitfield & MIL_ADC_PIN11_bm){

        GPIOPinTypeADC(GPIO_PORTB_BASE, GPIO_PIN_5);

    }

}

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
 * Parameters: ENSURE STEP NUM AND NUMBER OF 1S IN BITFIELD ARE EQUAL OTHERWISE,THE FUNCTION WILL BREAK
 * adc_base - from TIVA library either ADC0_BASE or ADC1_BASE
 * Seq_num - which sequence you wish to enable(see Sequence defines in this header)
 * pin_bitfield - what pins you're using(see pin defines in this header)
 * trig - from mil_trig_t, this specifies what triggers your ADC
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
 */
mil_adc_stat_t MIL_ADCSeqInit(uint32_t base,uint8_t seq_num,uint16_t pin_bitfield,mil_trig_t trig){

   if(base == ADC0_BASE){

       SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
       SysCtlPeripheralReset(SYSCTL_PERIPH_ADC0);

   }
   else if(base == ADC1_BASE){

       SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC1);
       SysCtlPeripheralReset(SYSCTL_PERIPH_ADC1);

   }
   else{

       return MIL_ADC_NOK;
   }

    pin_bitfield = pin_bitfield & 0x0FFF; //get rid of extraneous bits

    uint32_t local_trig;

    //determine trigger source
    switch(trig){
        case MIL_ADC_SoftTrig:
            local_trig = ADC_TRIGGER_PROCESSOR;
            break;
        case MIL_ADC_TimTrig:
            local_trig = ADC_TRIGGER_TIMER;
            break;
        case MIL_ADC_AlwaysTrig:
            local_trig = ADC_TRIGGER_ALWAYS;
            break;
        default:
            local_trig = ADC_TRIGGER_PROCESSOR;
            break;
    }

    ADCSequenceConfigure(base,seq_num,local_trig,seq_num);

    uint8_t step_max;

    //generate max number of steps
    switch(seq_num){
        case MIL_ADC_SEQ0:
            step_max = 8;
            break;

        case MIL_ADC_SEQ1:
            step_max = 4;
            break;

        case MIL_ADC_SEQ2:
            step_max = 4;
            break;

        case MIL_ADC_SEQ3:
            step_max = 4;
            break;
        default:
            step_max = 1;
            break;
    }

    uint8_t step_need = 0;
    uint16_t temp_field = pin_bitfield;

    //count needed steps based on bitfield
    while(temp_field){

        //check each bit for a 1,
        //if there's a 1 ,then there's a step needed for that channel
        if(temp_field & 0x01){

            step_need++;

        }

        //shift bits into the lsb,when this hits all 0s ,the function will end
        temp_field = temp_field >> 1;


    }

    uint8_t actual_steps;

    if(step_need > step_max){

        actual_steps = step_max;

    }
    else{

        actual_steps = step_need;

    }


    temp_field = pin_bitfield;
    uint8_t channel = 0;

    //step configurations
    /*
     * in effect this function configures each step by
     * assign each sequential step to the next available channel
     */
    for(uint8_t i = 0;i < actual_steps;i++){

        temp_field = pin_bitfield & (0x01<<channel);

        //if field = 0 after the bit wise and,
        //generate a new field
        while(!temp_field){

            channel++;
            temp_field = pin_bitfield & (0x01<<channel);

            if(channel > 11){

                return MIL_ADC_NOK;

            }

        }

        uint32_t config_field = channel; //set channel and enable interrupts for that step

        //if this is the last step, configure it as so by adding the
        //End flag
        if((i+1) == actual_steps){

            config_field |= ADC_CTL_END | ADC_CTL_IE;

        }

        ADCSequenceStepConfigure(base, seq_num, i, config_field);

        channel++;

    }


    ADCSequenceEnable(base,seq_num);

    ADCIntClear(base,seq_num);

    return MIL_ADC_OK;
}

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
void MIL_ADCIntEnable(void (*isr_ptr)(void),uint32_t base,uint8_t seq_num){

    ADCIntRegister(base,seq_num,isr_ptr);

    ADCIntEnable(base,seq_num);

}

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
mil_adc_stat_t MIL_ADCGetData(uint32_t base,uint8_t seq_num,uint32_t timeout,uint32_t *pbuffer){

    uint32_t timeout_cnt = 0;

    //wait for new adc data
    while(!ADCIntStatus(base,seq_num,false)){

        timeout_cnt++;
        if(timeout_cnt == timeout){

            return MIL_ADC_NOK;

        }

    }

    //get data
    ADCSequenceDataGet(base,seq_num,pbuffer);

    //clear interrupt
    ADCIntClear(base,seq_num);

    //if you've reached here, there was data therefore OK
    return MIL_ADC_OK;

}
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
float MIL_ADC_HextoFloat(uint16_t adc_value){

    float voltage;
    float slope = 3/0xFFF;

    voltage = adc_value * slope;
    return voltage;
}

/*
 * Desc: just incase you needed to convert the ADC values to ascii
 *       This function will take your 12 bit ADC value and output
 *       a string including its raw hex values
 *
 * Note: outstring is 3 bytes long
 *
 * Note: I used this for debugging
 */
void MIL_ADC_HEXtoASCII(uint16_t adc_value,char *pstring){

    char hex_char[3];

    //isolation
    hex_char[0] = (adc_value>>8) & 0x000F;
    hex_char[1] = (adc_value>>4) & 0x000F;
    hex_char[2] =  adc_value     & 0x000F;

    //conversion to ascii
    for(uint8_t idx = 0; idx < 3;idx++){

        if(hex_char[idx] > 9){

            hex_char[idx] = hex_char[idx] + 0x37;

        }
        else{

            hex_char[idx] = hex_char[idx] + 0x30;

        }
        //copy to outstring
        pstring[idx] = hex_char[idx];
    }

}

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
void MIL_ADC_FloattoASCII(uint16_t adc_value,char *pstring){

     double result_double = (double)adc_value;

     if(result_double < 0){
            result_double = result_double * -1;
     }

     double org_res = (result_double*3)/4095;
     int8_t res_int = (int8_t)org_res;           //first int
     double res1 = 10*(org_res - res_int);
     int8_t res_int1 = (int8_t)res1;             //second int
     double res2 = 10*(res1 - res_int1);
     int8_t res_int2 = (int8_t)res2;             //third int

     //conversion to char
     pstring[0]  = res_int  + 0x30;
     pstring[1] = '.';
     pstring[2] = res_int1 + 0x30;
     pstring[3] = res_int2 + 0x30;

}

