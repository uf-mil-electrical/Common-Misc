Name: MIL_SPI
Author: Marquez Jones
Desc: Wrapper functions for using SPI on the TM4C123GH6PM
NOTE: I recommend turing optimization on while using these functions just due to the the 
      TX and RX functions having additonal logic on top of the TivaWare base function. I haven't 
      read the compiler documentation ,but I'm going to make the educated guess that the extra
      switch statement will get optimized out when it realizes the port parameter is constant
