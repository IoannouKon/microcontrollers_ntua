.include "m328PBdef.inc"
.equ FOSC_MHZ=16
.equ DEL_MS=500
.equ DEL_NU=FOSC_MHZ*DEL_MS
.org 0x00
    rjmp reset
.org 0x04
    rjmp ISR1
reset:
    ldi r24 , low(RAMEND)
    out SPL , r24
    ldi r24, high(RAMEND)
    out SPH , r24
    ser r26
    out DDRC,r26                    ;In PORTC we show the number of interrupts
    out DDRB,r26		    ;In PORTB we show the counter of the main programm
    ldi r24,(1 << ISC11)| (1<<ISC10);Interrupt 1 may occur in the rise of PD3 signal 
    sts EICRA, r24
    ldi r24,(1<<INT1)               ;We enable the mask of interrupt1
    out EIMSK,r24
    sei                             ;We enable the global register of interrupts
    loop1:
	clr r26                     ;r26 is used as counter for the mai programm
    loop2:
	out PORTB, r26
	ldi r24, low(DEL_NU)
	ldi r25, high(DEL_NU)  
	nop
    	rcall delay_mS
	inc r26
	cpi r26,32
	brne loop2	
        rjmp loop1
                         
delay_mS:                         ;It causes a delay of 1000*x+3
    ldi r23, 249
    loop_inn:
     dec r23
     nop
     brne loop_inn
     sbiw r24,1
     brne delay_mS     
ret
     
ISR1:
    push r21          ;We save all the registers that are affected in this
                      ;interrupt because an interrupt may occur.
    push r22
    push r23
    push r24
    push r25
    in r21,SREG       ;We save the status register for the main programm
    push r21

    loop3:
    ldi r24,low(5*16) ;We set a delay for the arcing problem
    ldi r25,high(5*16)
    ldi r22,(1<<INTF1) ;We set the register EIFR(1)=0 (inverted logic) to check if it will be 1 after a delay
    out EIFR,r22
    rcall delay_mS
    in r22,EIFR
    andi r22,2         ;We use a mask to check the EIFR(1) bit
    brne loop3         ;If it is 0 then there is no arcing problem and we continue, but if it is not we repeat this until its 0
    in r19,PIND       ;We need to check if PD7 is pressed
    andi r19,128       ;We use a mask to isolate the 7nth bit
    breq END           ;If it is not zero then we press PD7 and the counetr of interupts doew not increase
    inc r20            ;We increase the counter
    cpi r20,32         ;We count till 31 
    brne NEXT
    clr r20            ;If we go until 32 we set our counter 0
    NEXT:
    out PORTC,r20
    END:
    pop r21            ;We restore the registers
    out SREG,r21
    pop r25
    pop r24
    pop r23
    pop r22
    pop r21
    sei               ;We enable the interrupts             
    reti
