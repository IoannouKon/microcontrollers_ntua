#define F_CPU 16000000UL
#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/sfr_defs.h>
#include <string.h>
#include <stdint.h>

#define En 1 //enable
#define Rs 0 

int ADSC_check;
int LCD_init()
{
    _delay_ms(40);
    PORTD = 0b100100; //4bit mode 1-line 5x11
    PORTD = 0b100100;
    _delay_us(40);
    command(0x0C); //hide cursor: display ON 
    command(0x01); // clear display
    _delay_us(1530);
    command(0x06); // increase LCD pointer automatically by one
    return 0;
}
void blink(int x)
{
	switch(x)
	{
	case 0:
	PORTB = 0b011111;
    _delay_ms(100);
	break;
	
	case 1:
	PORTB = 0b001111;
	_delay_ms(50);
	PORTB = 0xFF;
	_delay_ms(50);
	break;
	
	case 2:
	PORTB = 0b000111;
	_delay_ms(50);
	PORTB = 0xFF;
	_delay_ms(50);
	break;
	
	case 3:
	PORTB = 0b000011;
	_delay_ms(50);
	PORTB = 0xFF;
	_delay_ms(50);
	break;
	
	case 4:
	PORTB = 0b000001;
	_delay_ms(50);
	PORTB = 0xFF;
	_delay_ms(50);
	break;
	
	case 5:
	PORTB = 0;
	_delay_ms(50);
	PORTB = 0x111111;
	_delay_ms(50);
	break;
	}
}

int ADC_check()
{
	ADCSRA = 0b10000111;
	ADMUX  = 0b01000011;
	ADSC_check = 1;
    float value;
	while(1)
    {
		if (ADSC_check)
		{
			bit_is_set(ADCSRA, ADSC); // C3:: start converting voltage on A0
			ADSC_check = 0;
		}
		if (bit_is_clear(ADCSRA, ADSC))
		{
			value = (ADC*5)/1024; 
			ADSC_check = 1;
			return value;
		}
	}
    value = (value*5)/(2^(10));
    return value;
}

void Pulse()
{
	bit_is_set(PORTB,En); // take LCD enable line high
	_delay_us(40); // wait 40 microseconds
	bit_is_clear(PORTB,En); // take LCD enable line low
} 

int write_2_nibbles(uint8_t data)
{
	PORTB &= 0xC3; // 1100.0011
	if(data & 0b10000) bit_is_set(PIND,2);
	if(data & 0b100000) bit_is_set(PIND,3);
	if(data & 0b1000000) bit_is_set(PIND,4);
	if(data & 0b10000000) bit_is_set(PIND,5);
	Pulse(); 
    return 0;
}

int send_2_nibbles(uint8_t data)
{
	write_2_nibbles(data);	 // send upper 4 bits
	write_2_nibbles(data<<4); // send lower 4 bits
	bit_is_clear(PORTB,5);
    return 0;
}

void command (uint8_t cmd)
{
	bit_is_clear(PORTB,Rs); 
	send_2_nibbles(cmd); 
}
void write_2_lcd (uint8_t x)
{
	bit_is_set(PORTB,Rs);
	send_2_nibbles(x); 
}

void display_string(const char *txt) // display string on LCD
{
	
	while (*txt)
	{
		write_2_lcd(*txt++); 
	}

}
void Clear() // clear the LCD display
{
	command(0x01);
	_delay_ms(5); 
}

gas_level(int ADC_val){
	
	if(ADC_val < 0.1)
	{
		display_string("CLEAR");
		blink(0);
	}

	display_string("GAS DETECTED");

	if(ADC_val < 1)
	{

		blink(1);
	}

	if(ADC_val < 2)
	{
		blink(2);
	}

	if(ADC_val < 3)
	{
		blink(3);
	}

	if(ADC_val < 4)
	{
		blink(4);
	}

	if(ADC_val >= 5)
	{
		blink(5);
	}
}
int main(int argc, char** argv) {
	DDRB = 0xFF;
	DDRC = 0x00;
	DDRD = 0x00;
	PORTB = 1;
	int ADC_val;
	LCD_init();
	while(1)
	{
		
		ADC_val = ADC_check();
		gas_level(ADC_val);
		
	}

	return (EXIT_SUCCESS);
}

