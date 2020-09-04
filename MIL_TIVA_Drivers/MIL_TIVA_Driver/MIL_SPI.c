/*
 * Name: MIL SPI
 * Author: Marquez Jones
 * Desc: Wrappers for using SPI on the TIVA
 *
 * PLEASE REVIEW THE SPI OF THE SLAVE CHIP BEFORE USING THESE DRIVERS
 *
 * NOTE: The TIVA doesn't have an SPI dedicated module.
 *       Instead is has SSI(Synchronous Serial Interface) which
 *       has traditional SPI
 *
 * CHIP SELECT NOTES: The SSI module only has control over one chip select/slave select line
 *                    This is fine if you only have one spi chip on your board otherwise you'll need
 *                    to come up with how you want to handle the design.
 *
 *                    There's two solutions:
 *                    Solution A: you use the one chip select and use some kind of external decoding
 *                                to handle selection(Demux or Decoder chip along with other IO)
 *                    Solution B: you don't manually control the chip selections using IO
 *
 */

/*
 * Enable the spi module
 * Params:
 * port : from the mil port enum
 * role : slave or master from the role
 * clk_freq : clock speed of data transfer
 * cs_mode : whether you're using the module controlled slave select or manual selection
 * data_len : data transfer length
 *
 *
 */

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/ssi.h"
#include "driverlib/sysctl.h"


#include"MIL_SPI.h"

void MIL_SPI_Init(mil_spi_port_t port,mil_spi_role_t role,uint32_t clk_freq,
                    mil_spi_cs_mode_t cs_mode,uint32_t data_len){

    uint32_t base;
    uint32_t role_sel = SSI_MODE_MASTER;

    if(role == MIL_SPI_SLAVE){
        role_sel = SSI_MODE_SLAVE;
    }

    switch(port){
        case MIL_SPI_PORTA_MOD0:
            base = SSI0_BASE;
            SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);
            SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
            GPIOPinConfigure(GPIO_PA2_SSI0CLK);
            GPIOPinConfigure(GPIO_PA3_SSI0FSS);
            GPIOPinConfigure(GPIO_PA4_SSI0RX);
            GPIOPinConfigure(GPIO_PA5_SSI0TX);
            GPIOPinTypeSSI(GPIO_PORTA_BASE,
                           GPIO_PIN_5 | GPIO_PIN_4 | GPIO_PIN_3 | GPIO_PIN_2);
            break;

        case MIL_SPI_PORTB_MOD2:
            base = SSI2_BASE;
            SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI2);
            SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
            GPIOPinConfigure(GPIO_PB4_SSI2CLK);
            GPIOPinConfigure(GPIO_PB5_SSI2FSS);
            GPIOPinConfigure(GPIO_PB6_SSI2RX);
            GPIOPinConfigure(GPIO_PB7_SSI2TX);
            GPIOPinTypeSSI(GPIO_PORTB_BASE,
                           GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
            break;

        case MIL_SPI_PORTD_MOD1:
            base = SSI1_BASE;
            SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI1);
            SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
            GPIOPinConfigure(GPIO_PD0_SSI1CLK);
            GPIOPinConfigure(GPIO_PD1_SSI1FSS);
            GPIOPinConfigure(GPIO_PD2_SSI1RX);
            GPIOPinConfigure(GPIO_PD3_SSI1TX);
            GPIOPinTypeSSI(GPIO_PORTD_BASE,
                           GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
            break;

        case MIL_SPI_PORTD_MOD3:
            base = SSI3_BASE;
            SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI3);
            SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
            GPIOPinConfigure(GPIO_PD0_SSI3CLK);
            GPIOPinConfigure(GPIO_PD1_SSI3FSS);
            GPIOPinConfigure(GPIO_PD2_SSI3RX);
            GPIOPinConfigure(GPIO_PD3_SSI3TX);
            GPIOPinTypeSSI(GPIO_PORTD_BASE,
                           GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
            break;

        case MIL_SPI_PORTF_MOD1:
            base = SSI1_BASE;
            SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI1);
            SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
            GPIOPinConfigure(GPIO_PF2_SSI1CLK);
            GPIOPinConfigure(GPIO_PF3_SSI1FSS);
            GPIOPinConfigure(GPIO_PF0_SSI1RX);
            GPIOPinConfigure(GPIO_PF1_SSI1TX);
            GPIOPinTypeSSI(GPIO_PORTF_BASE,
                           GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_0 | GPIO_PIN_1);
            break;


    }

    SSIConfigSetExpClk(base, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0,
                       role_sel,clk_freq, data_len);

    SSIEnable(base);



}

//Wrappers to document intent to use SPI
/*
 * Your other option is to use the build in SSIData functions but I got rid of the
 * base variables in favor of the enum above
 */
void MIL_SPIDataGet(mil_spi_port_t port,uint32_t *pData){


    uint32_t base;
    switch(port){

        case MIL_SPI_PORTA_MOD0:
            base = SSI0_BASE;
            break;
        case MIL_SPI_PORTB_MOD2:
            base = SSI2_BASE;
            break;
        case MIL_SPI_PORTD_MOD1:
            base = SSI1_BASE;
            break;
        case MIL_SPI_PORTD_MOD3:
            base = SSI3_BASE;
            break;
        case MIL_SPI_PORTF_MOD1:
            base = SSI1_BASE;
            break;

    }
    SSIDataGet(base, pData);



}
void MIL_SPIDataPut(mil_spi_port_t port,uint32_t data){


    uint32_t base;
    switch(port){

        case MIL_SPI_PORTA_MOD0:
            base = SSI0_BASE;
            break;
        case MIL_SPI_PORTB_MOD2:
            base = SSI2_BASE;
            break;
        case MIL_SPI_PORTD_MOD1:
            base = SSI1_BASE;
            break;
        case MIL_SPI_PORTD_MOD3:
            base = SSI3_BASE;
            break;
        case MIL_SPI_PORTF_MOD1:
            base = SSI1_BASE;
            break;

    }
    SSIDataPut(base, data);



}
