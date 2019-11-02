Name: MIL_CLK \
Author: Marquez Jones \
Date Created: 3/13/2019 \
Desc: This includes functions to configure the system clock \

Reason for existence: \
In effect, the default example code in for 
TivaWare has this section of code. \

```
    SysCtlClockSet(SYSCTL_SYSDIV_1 |
                   SYSCTL_USE_OSC  |
                   SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_16MHZ);

```

IF YOU SEE THIS I,MMEDIATELY REMOVE IT.
THIS CODE REQUIRES AN EXTERNAL OSCILLATOR AND 
WILL CAUSE THE CORE TO LOCK UP. 

Most PCB designs in MIL don't include an external oscillator due to design simplication. 
If your board has an oscillator on the MOSC pins, then this code will run fine otherwise use the function in this 
header to initialize this clock. 

USE THIS FUNCTION AT THE BEGINNING OF YOUR MAIN
```
 MIL_ClkSetInt_16MHz();

```

In case of core lockup follow the unlocking section of the JTAG Interface manual(section 5.3.1) of the spma075 manual.

