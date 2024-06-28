;Askisi 1.3

.include "m328PBdef.inc"



reset:    
    ldi r24 , low(RAMEND)       ; initialize stack pointer
    out SPL , r24
    ldi r24 , high(RAMEND)
    out SPH , r24
    ser r24                        ; initialize PORTD for output
    out DDRD , r24
    ldi r26 , 1                 ; set r26 as 00000001, output register
    ser r27                        ; direction regisyer (0:left or 1:right)   [Τ flag του SREG.]

main:    
    out PORTD , r26    ; show leds 
    rcall WAIT 

    cpi r27 , 0
    breq sleft        ;jump if left else continue
     
    set 
    lsr r26
    rjmp main

sleft:
    clt
    lsl r26
    rjmp main

check_boundaries:
    cpi r26 , 0b00000001
    breq change_direction
    cpi r26 , 0b10000000
    breq change_direction
    ret

change_direction:
    com r27
    //set  ; T flag 1 
    jmp wait15 
    
    jmp  main 


WAIT: 
  rcall check_boundaries
 // clt  ; T flag 0 
  jmp wait05
  



wait05: 
	push r24	    ;2usec
	push r25
	ldi r24, low(490)
	ldi r25, high(490)
    delay05_start:
	sbiw r24, 1	;2usec
	breq leave05	;1usec xoris alma 2usec me alma
	rcall usec05	    ;3usec
	rjmp delay05_start    ;2usec
	    
    leave05:
	pop r25		    ;2usec
	pop r24	
	ret			;4usec			21usec total to end

	
    usec05:
    push r24	    ;2usec
	push r25
	ldi r24, low(199) ;1usec
	ldi r25, high(199);1usec          9 21usec total from start
	inc r24
    delay05:
	sbiw r24, 1	    ;2usec
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;DELAY;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	nop		    ;1usec
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;	
	brne delay05	    ;1usec xoris alma 2usec me alma
	nop
	pop r25		    ;2usec
	pop r24	
	ret		    ;4usec

wait15:
    push r24	    ;2usec
	push r25
	ldi r24, low(980)
	ldi r25, high(980)
    delay15_start:
	sbiw r24, 1	;2usec
	breq leave15	;1usec xoris alma 2usec me alma
	rcall usec15	    ;3usec
	rjmp delay15_start    ;2usec
	    
    leave15:
	pop r25		    ;2usec
	pop r24	
	ret			;4usec			21usec total to end

	
    usec15:
    push r24	    ;2usec
	push r25
	ldi r24, low(199) ;1usec
	ldi r25, high(199);1usec          9 21usec total from start
	inc r24
    delay15:
	sbiw r24, 1	    ;2usec
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;DELAY;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	nop		    ;1usec
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;	
	brne delay15	    ;1usec xoris alma 2usec me alma
	nop
	pop r25		    ;2usec
	pop r24	
	ret		    ;4usec




