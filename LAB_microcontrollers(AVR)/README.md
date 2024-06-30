
## Table of Contents
- [Introduction](#exercise-1)
- [Interupts](#exercise-2)
- [Timers and PWM](#exercise-3)
- [ADC and LCD screen](#exercise-4)
- [Εxternal expansion ports in AVR](#exercise-5)
- [Keyaboard 4*4 in expansion port](#exercise-6)
- [Exercise 7](#exercise-7)
- [Exercise 8](#exercise-8)

###  [Introduction](./Exercise_1) 
Introductory Exercise in which we simply implement a program in AVR Assembly that creates a "wagon" where one bit of the port D shifts from the MSB to the LSB and vice versa. Familiarization with MPLABX and live expressions for debugging.

###  [Interupts](./Exercise_2) 

## Using Interrupts with AVR Assembly and C 
### 2.1
In this exercise, we implement an interrupt for a switch (PD7) that toggles a counter: the counter increments when the switch is pressed and stops when pressed again. We handle switch debouncing by clearing the External Interrupt Flag Register (EIFR). 
### 2.2 
![Implementation of a flow diagram](https://github.com/IoannouKon/microcontrollers_ntua/assets/132226067/10d71647-d349-4314-a491-5405e17c7b35)
### 2.3
In this exercise, we implement an interrupt for a switch (PD3, corresponding to interrupt INT1) that turns on an LED when pressed. The LED stays on for 4 seconds and then turns off unless PD3 is pressed again, which resets the timer for another 4 seconds. This reset is indicated by the LED blinking for 0.5 seconds.


###  [Timers and PWM](./Exercise_3) 

### 3.1 Timers and Interupts in AVR assembly and C 
![Implementation of a flow diagram](https://github.com/IoannouKon/microcontrollers_ntua/assets/132226067/3809acd2-ee20-45e2-b709-d6683e16c3af)

### 3.2 Generating PWM Waveform with AVR  in AVR assembly and C 
In this exercise, we generate an 8-bit PWM waveform on pin PB1 to control the brightness of an LED. The initial duty cycle is set to 50%. Pressing PD1 increases the duty cycle up to 98%, making the LED brighter, while pressing PD3 decreases the duty cycle down to 2%, dimming the LED. This exercise includes configuring and adjusting the PWM signal using interrupts to handle the button presses.

### 3.3 Play piano with PWM and buzzer in AVR assembly and C 
In this exercise, when no button is pressed, no waveform is generated on PB1, so no sound is produced. Each button PD0-PD3 has its own frequency.To do that we just continoues read the PIND we dont use interrupts.So now you can play Super Mario theme if you know the frequency.

###  [ADC and LCD screen](./Exercise_4) 
### 4.1 and 4.2 ADC with Polling and Interupts  
In this exercise, the goal is to read the analog value from the potentiometer on the board using either polling (continuous waiting) or interrupt techniques. Then, we convert this value to digital and display it on the LCD screen with a precision of two decimal places.In C we use polling and in Assembly interupts.

### 4.3 PWM and ADC polling and LCD Screen 
In tis exersice  e use PWM as analog input for adc so with PB port we choose what duty cycle we wiil have then we read it with  polling and conver it to digital so we can print in LCD screen  with a precision of two decimal.

###  [Εxternal expansion ports in AVR](./Exercise_5)   
When press "*" on led PD0 
When press "0" on led PD1
When press "#" on led PD2
When press "D" on led PD3
else all leds off!

![PCA9555D in ntuAboard_G1.](https://github.com/IoannouKon/microcontrollers_ntua/assets/132226067/8abf64fb-eeb3-48c7-9ffb-2a4d1cca5444)


###  [Keyaboard 4*4 in expansion port](./Exercise_6) 

In AVR C we have two exersice 
6.1)Read the pressed button and display it on the screen!
6.2)Read Code example: Enter a two-digit code on the board's keypad. If you enter the wrong code, LEDs PB0-PB5 will blink for 5 seconds, and during this time you cannot input a new character via the keypad. If you enter the correct code, LEDs PB0-PB5 remain lit continuously for 4 seconds!


