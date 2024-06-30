
## Table of Contents
- [Exercise 1](#exercise-1)
- [Exercise 2](#exercise-2)
- [Exercise 3](#exercise-3)
- [Exercise 4](#exercise-4)
- [Exercise 5](#exercise-5)
- [Exercise 6](#exercise-6)
- [Exercise 7](#exercise-7)
- [Exercise 8](#exercise-8)

###  [Exercise 1](./Exercise_1) 
Introductory Exercise in which we simply implement a program in AVR Assembly that creates a "wagon" where one bit of the port D shifts from the MSB to the LSB and vice versa. Familiarization with MPLABX and live expressions for debugging.

###  [Exercise 2](./Exercise_2) 

## Using Interrupts with AVR Assembly and C 
### 2.1
In this exercise, we implement an interrupt for a switch (PD7) that toggles a counter: the counter increments when the switch is pressed and stops when pressed again. We handle switch debouncing by clearing the External Interrupt Flag Register (EIFR). 
### 2.2 
![Implementation of a flow diagram](https://github.com/IoannouKon/microcontrollers_ntua/assets/132226067/10d71647-d349-4314-a491-5405e17c7b35)
### 2.3
In this exercise, we implement an interrupt for a switch (PD3, corresponding to interrupt INT1) that turns on an LED when pressed. The LED stays on for 4 seconds and then turns off unless PD3 is pressed again, which resets the timer for another 4 seconds. This reset is indicated by the LED blinking for 0.5 seconds.
