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

#include "driverlib/ssi.h"

#ifndef MIL_SPI_H_
#define MIL_SPI_H_

/*
 *Desc: Port selection will come from this enum
 *      mod = module
 *
 * PORTA_MOD0 PINS:
 * 2 CLK
 * 3 FSS/CS
 * 4 RX
 * 5 TX
 *
 * PORTB_MOD2 PINS:
 * 4 CLK
 * 5 FSS/CS
 * 6 RX
 * 7 TX
 *
 * PORTD_MOD1 PINS:
 * 0 CLK
 * 1 FSS/CS
 * 2 RX
 * 3 TX
 *
 * PORTD_MOD3 PINS:
 * 0 CLK
 * 1 FSS/CS
 * 2 RX
 * 3 TX
 *
 * PORTF_MOD1 PINS:
 * 2 CLK
 * 3 FSS/CS
 * 0 RX
 * 1 TX
 *
 */
typedef enum {
    MIL_SPI_PORTA_MOD0,
    MIL_SPI_PORTB_MOD2,
    MIL_SPI_PORTD_MOD1,
    MIL_SPI_PORTD_MOD3,
    MIL_SPI_PORTF_MOD1
}mil_spi_port_t;

typedef enum{
    MIL_SPI_MASTER = 0,
    MIL_SPI_SLAVE = 1
}mil_spi_role_t;

typedef enum{
    MIL_CS_MOD_CTRL, //chip select controlled by module
    MIL_CS_MAN_CTRL  //chip select not controlled by module
}mil_spi_cs_mode_t;

/*
 * Enable the spi module
 * Params:
 * port : from the mil port enum
 * role : slave or master from the role
 * clk_freq : clock speed of data transfer
 * cs_mode : whether you're using the module controlled slave select or manual selection
 * data_len : data transfer length
 *
 * CHECK SIGNAL TABLE IN TIVA MANUAL TO KNOW WHICH PINS WILL BE USED
 *
 *
 */
void MIL_SPI_Init(mil_spi_port_t port,
                    mil_spi_role_t role,
                    uint32_t clk_freq,
                    mil_spi_cs_mode_t cs_mode,
                    uint32_t data_len);

//Wrappers to document intent to use SPI
/*
 * Your other option is to use the build in SSIData functions but I got rid of the
 * base variables in favor of the enum above
 */
void MIL_SPIDataGet(mil_spi_port_t port,uint32_t *pData);
void MIL_SPIDataPut(mil_spi_port_t port,uint32_t data);



#endif /* MIL_SPI_H_ */
