#include <avr/io.h>f

char a,b,c,d,cb,cc,cd,f,g;

int main(void){
	DDRB=0x00;  //b=input
	DDRA=0xFF;  //a=output
	
	while(1){
		a = PINB & 0x01;
		b = PINB & 0x02;
		b = b>>1;
		cb = (~PINB) & 0x02;
		cb = cb>>1;
		c = PINB & 0x04;
		c = c>>2;
		cc = (~PINB) & 0x04;
		cc = cc>>2;
		d = PINB & 0x08;
		d = d>>3;
		cd = (~PINB) & 0x08;
		cd = cd>>3;

		f = (a & b & c) | (cb & cc & cd);
		g = (a | b | c) & d;
		g = g<<1;

		PORTA = f | g;
		//PORTA = b;
	}
	return 0;
}
