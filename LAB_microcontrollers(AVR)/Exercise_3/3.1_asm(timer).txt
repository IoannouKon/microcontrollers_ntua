    
    .include "m328PBdef.inc"
    .equ FOSC_MHZ=16 
    .org 0x0
    rjmp reset
    .org 0x4
      rjmp ISR1  
    .org 0x1A
    rjmp ISR_TIMER1_OVF      


    reset:
	ldi r24, (1<<ISC11)|(1<<ISC10)
	sts EICRA, r24
	ldi r24, (1<<INT1)
	out EIMSK, r24 
        sei 


	ser r25 
	out DDRB ,r25
	//out PORTB,r25
	 
	clr  r25
	out DDRD ,r25
	out DDRC ,r25
	//out PORTB,r25
	
	clr r26    // r26: 0 of all led is off / 1 if led is on 
	//clr r22  // r22: 0 wait dont go to timer_1 / 1 go now to timer_1
	
	


      start:
        sei
	clr r25
	in r25 ,PINC 
	cli
	andi r25 , 0b000100000
	cpi  r25 , 0b000100000 
	breq start            // breq start
	jmp ISR1
        
	
 
      timer_1:
       sei

       ldi r24,(1<<CS12)|(0<<CS11)|(1<<CS10) 
       sts TCCR1B ,r24

       cli
       ldi r24,HIGH(3035)
       sts  TCNT1H,r24
       ldi r24,LOW(3035)
       sts  TCNT1L,r24
      
       
       ldi r24,(1<<TOIE1) ;TCNT1
       sts TIMSK1 ,r24    ; timer1
       sei
       
       cli
       jmp  L1


    timer_2:  
	 ser  r25 
	 out PORTB ,r25 
	 cli

       ldi r24,(1<<CS12)|(0<<CS11)|(1<<CS10) 
       sts TCCR1B ,r24
       ldi r24,HIGH(58000)
       sts  TCNT1H,r24
       ldi r24,LOW(58000)
       sts  TCNT1L,r2
       
       ldi r24,(1<<TOIE1) ;TCNT1
       sts TIMSK1 ,r24    ; timer1  

       sei 
       ldi r22 , 0 
       jmp D_loop  // for some delay 0_5 sec 
       //jmp timer_1

       ISR_TIMER1_OVF:
        clr r25  //ser r25  
	 out PORTB ,r25 
	 ldi r26,0
	 ldi r22 ,1 
	 reti 
	 //jmp start 

	
     ISR1:    
	cpi  r26 ,  0  
	brne  timer_2    
	jmp   timer_1  
	
      
L1:
      
       ldi r26 , 1 
       ldi r30 ,0b0001
       out PORTB ,r30
       jmp start

  D_loop: 
        cpi r22 , 0
	brne timer_1 
	jmp D_loop
       
       
End:
    
   


