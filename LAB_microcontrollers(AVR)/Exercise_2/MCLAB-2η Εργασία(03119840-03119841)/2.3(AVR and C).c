.include "m328PBdef.inc"
    
.org 0x0
rjmp reset
.org 0x4
rjmp ISR1

.equ FOSC_MHZ = 16
.equ DEL_mS =  4000//500
.equ DEL_NU = DEL_mS*FOSC_MHZ
    
reset:
    ldi r24, (1<<ISC11)|(1<<ISC10)
    sts EICRA, r24
    ldi r24, (1<<INT1)
    out EIMSK, r24
    ldi r24, (1<<INTF1)
    out EIFR, r24
    ser r24
    out DDRB, r24
    clr r20
    sei
main:
    rjmp main
    
delay_mS:
    ldi r23, 14
    nop
    nop
loop_inn:
    dec r23
    nop
    brne loop_inn
    sbiw r24, 1
    brne delay_mS
ret
    
ISR1:
    cpi r20, 0
    breq start_ 
    ldi r20, 8
    ldi r24, low(DEL_NU)	
    ldi r25, high(DEL_NU)
    ser r26
    out PORTB, r26 
    rcall delay_mS
    sei
    ldi r26, 1
    out PORTB, r26
    reti
start_:
    ldi r20, 8
loop_:
    ldi r26, 1
    out PORTB, r26 
    ldi r24, low(DEL_NU)	
    ldi r25, high(DEL_NU)
    sei
    rcall delay_mS
    dec r20
    cli
    cpi r20, 0
    brne loop_
    clr r20
    out PORTB, r20
reti


ASK3.C
/////////////////////////////////////////////////

/* 
 * File:   ASK3_C.c
 * Author: mgeor
 *
 * Created on 1 ????????? 2022, 11:43 ??
 */

#define F_CPU  16000000UL
#include <stdio.h>
#include <stdlib.h>
#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>

int x = 0;
ISR (INT1_vect)
{
    sei ();
    if( x > 0 )
    {
        x = 0x4;
        PORTB=0xFF;
        _delay_ms(500);
        exit(0);
    }
    x=0x4;
    while(x > 0){
        PORTB=0x01;
       _delay_ms(250);
        x-=0x1;
    }
    PORTB=0x00;
}

int main(int argc, char** argv)
{


//Interrupt on rising edge of INTO and INT1 pin
    EICRA= (1 << ISC11) | ( 1 << ISC10);
//Enable the INTO interrupt (PD2), INT1 interrupt (PD3)
    EIMSK= (1 << INT1);
    sei ();
    DDRB=0xFF;
	DDRC=0xFF;
	PORTC=0x00;
    while (1)
    {
        PORTB=0x00;
        _delay_ms(1);
        PORTB=0x00;	
    }
    return (EXIT_SUCCESS);
}

    
    
  

    
   
    
   
