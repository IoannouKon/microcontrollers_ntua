#define F_CPU 16000000UL
#include "avr/io.h"
#include<avr/interrupt.h>
#include<util/delay.h>
#include <stdio.h>
int h;

void LCD_print(unsigned int   x ) {

unsigned int temp;  //???? ?? 3 ????? ??? ??????? 
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

 
    lcd_command(0xC0);
        _delay_ms(10);
    
    lcd_data(ch1);
    lcd_data('.');
    lcd_data(ch2);
    lcd_data(ch3); 
    
        

} 

void PWM_level(int i)
{
    switch(i)
    {
        case 4:
            if(h!=i)
            {
                h=i;
            lcd_command(1);
            _delay_ms(10);
            display_string("PWM 0%");
            }
            break;

        case 0:
            if(h!=i)
            {
                h=i;
            lcd_command(1);
            _delay_ms(10);
            display_string("PWM 20%");
            }
            break;            
    
        case 1:
            if(h!=i)
            {
                h=i;
            lcd_command(1);
            _delay_ms(10);
            display_string("PWM 40%");
            }
            break;
        case 2:
            if(h!=i)
            {
                h=i;
            lcd_command(1);
            _delay_ms(10);
            display_string("PWM 60%");
            }
            break;
    
        case 3:
            if(h!=i)
            {
                h=i;
            lcd_command(1);
            _delay_ms(10);
            display_string("PWM 80%");
            }
            break;
    }
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
		{       value = (ADC*5)/1024;
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
	_delay_ms(10);
    lcd_command(0x28);
	_delay_ms(10);
    lcd_command(0x0c);
    _delay_ms(100);
	lcd_command(0x01);
	_delay_ms(500);
	lcd_command(0x06);
}



void display_string(const char *txt) // display string on LCD
{
	
	while (*txt)
	{
		lcd_data(*txt++); 
	}

}


///////////////////////////////////////////

int main()
{    
    //DDRB = 0x111  ;  
	int duty[5] = {80, 160, 240, 320, 0};
	TCCR1A = (1<<WGM10) | (1<<COM1A1);
	TCCR1B = (1<<WGM12) | (0<<CS12) | (1<<CS11) | (0<<CS10);
	DDRB |=(1<<PB1)|(1<<PB0);
	DDRD |= 0b11111111;  // output for LCD
    DDRC |= 0b00000000;  // input for ADC
    ADCSRA = 0b10000111;
	ADMUX  = 0b01000001;
    

    
    int i,x;
	unsigned int ADC_val;
    lcd_init();
    while(1)
        
	{
       
        x = PINB;
         i=4;
         ICR1 = 0;
         
        if((x & 0b000100) == 0)
        {
            i=0;    
         ICR1 = 400;       
        }
         if((x & 0b001000) == 0)
        {
             i=1;
         ICR1 = 400;       
        }
        if((x & 0b010000) == 0)
        {
              i=2;  
        ICR1 = 400;        
        }
        if((x & 0b100000) == 0)
        {
            i=3;
         ICR1 = 400;     
        }
        
		
        OCR1A = duty[i];
        PWM_level(i);
        
        lcd_command(0xC0);
        _delay_ms(10);
        display_string("                ");
        lcd_command(0xC0);
        _delay_ms(10);
         lcd_command(0xC0);
        _delay_ms(10);
        
        ADC_val = ADC_check();
        LCD_print(ADC_val);
        _delay_ms(250);
          }
	return 0;
}

