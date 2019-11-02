Name: MIL_BR_ESC
Author: Marquez Jones
Date Posted: 3/16/2019
Desc: This is a set of defines and accessory functions that basically abstract the out the logic
      of the Blue Robotics PWM system. This is for if your TIVA based board needed to 
      interface with the BR ESCs(Electronic Control Units)
      
Link to BR Basic ESC and Documentation here: https://www.bluerobotics.com/store/thrusters/speed-controllers/besc30-r3/

Note: Effectively, when interfacing PWM with the ESCs, it's not basic pwm in the sense that we're attempting to get some 
      duty cycle to the thruster itself. What occurs is that we send a "message" via PWM to the ESCs which will then get
      interpreted by the ESC which will then apply the correct thrust. 
      
      What this means is that when you send a 50% duty cycle to the ESC, it doesn't actually correlate to 50% thrust on the thruster.
      To give reference, here's some basic mapping that's also included in the header:
      
      95% duty - Full forward
      75% duty - Full Stop
      55% duty - Full Reverse
  
      The rule is any duty > 75% is forward
                  any duty < 75% is reverse

