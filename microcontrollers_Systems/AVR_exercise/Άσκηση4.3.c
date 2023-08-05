#include <avr/io.h>

int main (void)
{
	DDRB = 0xFF;	//port B as output
	DDRA = 0x00;    //port A as input
	
	int x = 0b00000001;		//output led

	while (1)
	{
		if (PINA & 0b00000001)			//SW0 pressed, circular sll
		{
			while (PINA & 0b00000001);	//wait until released
			if (x == 0b10000000) x = 0b00000001;	//circular sll, check for overflow
			else x = x << 1;
		}
		else if (PINA & 0b00000010)		//SW1 pressed, circular slr
		{
			while (PINA & 0b00000010);
			if (x == 0b00000001) x = 0b10000000;	//overflow check
			else x = x >> 1;
		}
		else if (PINA & 0b00000100)		//SW2 pressed, circular 2 step sll
		{
			while (PINA & 0b00000100);
			if (x == 0b10000000) x = 0b00000010;	//overflow check
			else if (x == 0b01000000) x = 0b00000001;
			else x = x << 2;
		}
		else if (PINA & 0b00001000)		//SW3 pressed, led to MSB
		{
			while (PINA & 0b00001000);
			x = 0b10000000;
		}
		PORTB = x;
	}
}
