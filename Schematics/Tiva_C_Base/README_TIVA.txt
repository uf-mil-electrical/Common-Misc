Author: Marquez Jones
Schematic Name:Tiva_C_Base
Desc: Documentation for Tiva Base Schematic.

Purpose:
	This schematic consists of the bare minimum connections for using the Tiva
on a PCB project. This is a non-task specific schematic to be imported to any project deploying
the Tiva processor. 

What's Included:
-Tiva power connections including decoupling capacitors
-Tiva reset switch circuit
-Tiva programming connections
-Mandatory on LED circuit

Programming:
	Provided in the schematic is access to JTAG pins via a header. The TI tiva launchpad can be used to program 
custom designed boards in MIL. This is accomplished by connecting the JTAG pins(TCK,TMS,TDI,TDO), a common GND, and the RESET to the pins on 
your designed board.

Using the TIVA style launchpad for programming/debugging:
-Read Debug Out Section
http://processors.wiki.ti.com/index.php/Stellaris_LM4F120_LaunchPad_Debug_How_To

