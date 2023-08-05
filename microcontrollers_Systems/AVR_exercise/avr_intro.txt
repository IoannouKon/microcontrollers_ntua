Παράδειγμα 1.1

.include "m16def.inc"

reset:    ldi r24 , low(RAMEND)   	; initialize stack pointer
out SPL , r24
	ldi r24 , high(RAMEND)
	out SPH , r24
	ser r24                    	; initialize PORTA for output
	out DDRA , r24
	clr r26                   	; clear time counter

main:    out PORTA , r26            
	ldi r24 , low(1000)         ; load r25:r24 with 1000 
	ldi r25 , high(1000)        ; delay 1 second
	rcall wait_msec
	inc r26                    	; increment time counter, one second passed
	cpi r26 , 16                	; compare time counter with 16
	brlo main                  	; if lower goto main, else clear time counter
	clr r26                   		; and then goto main
	rjmp main

wait_msec:
   	push r24		; 2 κύκλοι (0.250 μsec)
   	push r25		; 2 κύκλοι
 	ldi r24 , low(998)      	; φόρτωσε τον καταχ.  r25:r24 με 998 (1 κύκλος - 0.125 μsec)
  	ldi r25 , high(998)     	; 1 κύκλος (0.125 μsec)
   	rcall wait_usec        	; 3 κύκλοι (0.375 μsec), προκαλεί συνολικά καθυστέρηση 998.375 μsec 
				; Στην προσομοίωση δεν βάζουμε χρονοκαθυστέρηση!      
   	pop r25               	; 2 κύκλοι (0.250 μsec)
   	pop r24               	; 2 κύκλοι 
   	sbiw r24 , 1          	; 2 κύκλοι 
   	brne wait_msec        	; 1 ή 2 κύκλοι (0.125 ή 0.250 μsec)
   	ret			; 4 κύκλοι (0.500 μsec)

wait_usec:   
	sbiw r24 ,1      		; 2 κύκλοι (0.250 μsec)  
	nop           		; 1 κύκλος (0.125 μsec)
	nop          		; 1 κύκλος (0.125 μsec)
	nop           		; 1 κύκλος (0.125 μsec)
	nop           		; 1 κύκλος (0.125 μsec)
	brne wait_usec		; 1 ή 2 κύκλοι (0.125 ή 0.250 μsec)
    	ret             		; 4 κύκλοι (0.500 μsec)




Παράδειγμα 1.2 

reset:    ldi r24 , low(RAMEND)   	; αρχικοποίηση stack pointer
out SPL , r24
	ldi r24 , high(RAMEND)
	out SPH , r24

ser r26         			; αρχικοποίηση της PORTA
out DDRA ,  r26   		; για έξοδο 

flash:  	rcall  on  			; Άναψε τα LEDs
nop				; Να αντικατασταθούν κατάλληλα οι 2 εντολές nop  
nop				; για προσθήκη καθυστέρησης 200 ms

rcall off 			; Σβήσε τα LEDs
nop				; Να αντικατασταθούν κατάλληλα οι 2 εντολές nop
nop				; για προσθήκη καθυστέρησης 200 ms

rjmp flash  			; Επανέλαβε

; Υπορουτίνα για να ανάβουν τα LEDs
on:	ser r26				; θέσε τη θύρα εξόδου των LED
out PORTA , r26
ret				; Γύρισε στο κύριο πρόγραμμα

; Υπορουτίνα για να σβήνουν τα LEDs
off:	clr r26				; μηδένισε τη θύρα εξόδου των LED
out PORTA , r26
ret	



Παράδειγμα 1.3

#include <avr/io.h>

char x, y, z, k;

int main(void)
{
 
DDRB=0xFF; 			// Αρχικοποίηση PORTB ως output
	DDRD=0x00;			// Αρχικοποίηση PORTD ως input
	DDRA=0x00;			// Αρχικοποίηση PORTA ως input
 
	while(1) 
	{
		x = PIND & 0x0F;	// Απομόνωση PD3-PD0

		y = PIND & 0xF0;	// Απομόνωση PD7-PD4
		y = y >> 4;		// Μεταφορά ψηφίου στην ορθή του αξία 

z = PINA & 0x0F;	// Απομόνωση PΑ3-PΑ0


		k = PINA & 0xF0;	// Απομόνωση PΑ7-PΑ4
		k = k >> 4;		// Μεταφορά ψηφίου στην ορθή του αξία

		PORTB = (x+y+z+k);	// Υπολογισμός αθροίσματος και έξοδος στην PORTB

	}

	return 0;
}




Παράδειγμα 1.4 

#include <avr/io.h>

char x;

int main(void)
{
DDRB=0xFF; 		// Αρχικοποίηση PORTB ως output
DDRA=0x00; 		// Αρχικοποίηση PORTA ως input

x = 1;			// Αρχικοποίηση μεταβλητής για αρχικά αναμμένο led

while(1)
{
		if ((PINA & 0x01) == 1){ 		// Έλεγχος πατήματος push-button SW0
		
while ((PINA & 0x01) == 1); 	// Έλεγχος επαναφοράς push-button SW0
		
if (x==128)			// Έλεγχος υπεχείλισης
				x = 1;
		 	else 
				x = x<<1;		// Ολίσθηση αριστερά

		}
	
PORTB = x; 				// Έξοδος σε PORTB
}
return 0;
}
