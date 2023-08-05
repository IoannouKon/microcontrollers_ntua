#define F_CPU 16000000UL
#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>

int main(int argc, char** argv) {
    unsigned char duty = 128;
	 TCCR1A = (1<<WGM11) | (1<<COM1A1);  //  WGM[10:11] σε πόσα bit αρχικοποιούμε τον TMR1A  

        DDRB |=(1<<PB1);
	DDRD = 0;
	TCCR1B = (1<<WGM13) | (1<<WGM12) | (1<<CS11);  // CS[12-10] Prescaler (N)
        int x;
	while(1){
        
        x=PIND;
        
        if(x == 0b11111110) // 125 Hz !!!!
        {
               
               ICR1 = 16000; //   ICR1=TOP   , fpwm = flck/(N*TOP)
               OCR1A=8000;   //   dc = ICR1/OCR1A   
               
        }    
        else if(x == 0b11111101) // 250 Hz
        {
               
               ICR1 = 8000;
               OCR1A= 4000;
               
        }    
        else if(x == 0b11111011) // 500 Hz !!!
        {
               ICR1 = 4000;
               OCR1A= 2000;
               
        }       
        else if(x == 0b11110111) //  1000 Hz
        {
                
               ICR1 = 2000;
               OCR1A= 1000;
               
             
        }
        else
        {
             
                   ICR1 = 0;
                   OCR1A= 0;
               
        }

		
	}
	
    return (EXIT_SUCCESS);
}

