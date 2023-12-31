#define F_CPU 16000000UL
#include "avr/io.h"
#include<avr/interrupt.h>
#include<util/delay.h>
#include <stdio.h>


int val ;

void write_2_nibbles(char x)
{
	char y=PIND & 0x0f;
	char x1=x & 0xf0;
	x1=x1+y;
	PORTD=x1;
	PORTD=PORTD | (1<<PD3);
	PORTD=PORTD & (0<<PD3);
	x=x<<4 | x>>4;
	x=x & 0xf0;
	PORTD=x+y;
	PORTD=PORTD | (1<<PD3);
	PORTD=PORTD & (0<<PD3);
}

void lcd_data(char x)
{
	PORTD=PORTD | (1<<PD2);
	write_2_nibbles(x);
	_delay_us(40);
}

void lcd_command(char x)
{
	PORTD=PORTD | (0<<PD2);
	write_2_nibbles(x);
	_delay_us(40);
}

void lcd_init (void)
{
	_delay_ms(40);
	PORTD=0x30;
	PORTD=PORTD | (1<<PD3);
	PORTD=PORTD & (0<<PD3);
	_delay_us(38);
	PORTD=0x30;
	PORTD=PORTD | (1<<PD3);
	PORTD=PORTD & (0<<PD3);
	_delay_us(38);
	PORTD=0x20;
	PORTD=PORTD | (1<<PD3);
	PORTD=PORTD & (0<<PD3);
	_delay_us(38);
	lcd_command(0x28);
	lcd_command(0x0c);
	lcd_command(0x01);
	_delay_ms(500);
	lcd_command(0x06);
}


///////////////////////////////////////////////////////////////////////////////



void LCD_print(unsigned int   x ) {

unsigned int temp;  //βρες τα 3 ψηφία που θέλουμε 
temp = x ;
int d , d1,d2,d3;
d1= temp/100 ;
d = temp - (d1*100) ;
d2 = d/10 ;
d3 = d -(d2*10); 

unsigned  char ch1,ch2,ch3;
ch1 = d1 + '0';
ch2 = d2 + '0';
ch3 = d3 + '0';

 
    lcd_command(0x01);
    _delay_ms(3);
    lcd_data(ch1);
    lcd_data('.');
    lcd_data(ch2);
    lcd_data(ch3); 

} 




 
 unsigned int  ADC_check()
{

   int ADSC_check = 1;
    unsigned int value;
    unsigned  int temp1; 
   
	while(1)
    {
		if (ADSC_check)
		{
		
            //bit_is_set(ADCSRA, ADSC); // C3:: start converting voltage on A0
                        ADCSRA |= (1<<ADSC);
			ADSC_check = 0;
		}

		if (   ( ADCSRA & (1 << ADSC)) == 0 )  //when conv is over     //bit_is_clear(ADCSRA, ADSC) 
		 {      
                        value = (ADC*5)/1024;
                        value =100*value;
                        temp1  = (((ADC*5)%1024)*10)/1024;
                        value=value +(10*temp1);
                        temp1  = ((((ADC*5)%1024)*10)%1024)*10/1024;
                        value = value +temp1 ;
              
             ADSC_check = 1;
             return value;    
		  
		}
	}
   
}
 
 
 int main() {

    DDRD |= 0b11111111;  // output for LCD
    DDRC |= 0b00000000;  // input for ADC
    DDRB |= 0b11111111;  // output for counter
    
        ADCSRA = 0b10000111;
	ADMUX  = 0b01000010;

	unsigned int ADC_val;
    
        lcd_init();
    
	while(1)
	{
		
	ADC_val = ADC_check();
        LCD_print(ADC_val) ; 
        _delay_ms(500);
		
	}

	return 0; //(EXIT_SUCCESS);
}

   
    
    
   
