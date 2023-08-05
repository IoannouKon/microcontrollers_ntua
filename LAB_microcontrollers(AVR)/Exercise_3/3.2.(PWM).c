#define F_CPU 16000000UL
#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>

int main(){
	unsigned char duty[13] = {5, 22, 40, 58, 76, 95, 128, 133, 148, 166, 184, 202, 220};
	TCCR1A = (1<<WGM10) | (1<<COM1A1);
	TCCR1B = (1<<WGM12) | (1<<CS11);
	DDRB = 0b00111111;
	DDRD = 0b00000000;
	int x, i=6;
	while(1)
	{
        x=PIND;
        if((x==0b11111101) && (duty[i] != 220))
        {
                i++;
                _delay_ms(100);
        }
        
        if((x == 0b11111011) && (duty[i] != 5))
        {
                i--;
                _delay_ms(100);
        }
         

		OCR1A = duty[i];
	}
	
	return (EXIT_SUCCESS);
}

