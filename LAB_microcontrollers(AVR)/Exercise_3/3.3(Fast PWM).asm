.include "m328PBdef.inc"
.equ FOSC_MHZ=16
.equ DEL_MS=500
.equ DEL_NU=FOSC_MHZ*DEL_MS
.org 0x00
    rjmp reset
	

       .def temp = r20  
       .def duty = r21 
       clr r28
       
 reset:
         
	ser temp   
	out DDRB, temp   // B output   
	clr temp  
	out DDRD, temp   // D input   
	
	ldi temp ,(0<<WGM10) | (1<<WGM11) | (1<<COM1A1)
	sts TCCR1A, temp   
	ldi temp, (1<<WGM13)|(1<<WGM12) |  (0<<CS12) |   (1<<CS11) | (0<<CS10)    
	sts TCCR1B, temp
	rjmp main_
	
	

main_:
	in temp, PIND
	com temp
	
	cpi temp, 0b01  // PD0
	breq PWM_125
	
	cpi temp, 0b010 // PD1
	breq PWM_250
	
	cpi temp, 0b100 // PD2
	breq PWM_500
	
	cpi temp , 0b1000 //PD3
	breq PWM_1000

        ldi r24, HIGH(0) 
        sts OCR1AH, r24 
        ldi r24, LOW(0)
        sts OCR1AL, r24        
	
	ldi r18 , LOW(0x0)
	ldi r19 , HIGH(0x0)
	sts ICR1L , r18  
	sts ICR1H,  r19  
	rjmp  main_
	
PWM_125:
        ldi r19 , HIGH(16000)
	sts ICR1H,  r19 
	ldi r18 , LOW(16000)
	sts ICR1L , r18  
       
    
	ldi r19, HIGH(8000) 
	sts OCR1Ah , r19 
	ldi r18 , LOW(8000)
	sts OCR1Al,  r18
	jmp main_	
	
	
PWM_250:
        ldi r19, HIGH(8000)   
	ldi r18 , LOW(8000)
	
	sts OCR1Ah , r19  // r4 = 0x80
	sts OCR1Al,  r18 // r5 = 0x00 start from 50/100  
	
      
       
	ldi r18 , LOW(4000)
	ldi r19 , HIGH(4000)
	sts ICR1L , r18  
	sts ICR1H,  r19 
	jmp main_	
	
PWM_500:
        ldi r19, HIGH(4000) 
	ldi r18 , LOW(4000)
	
	sts OCR1Ah , r19  // r4 = 0x80
	sts OCR1Al,  r18 // r5 = 0x00 start from 50/100  
	
      
       
	ldi r18 , LOW(2000)
	ldi r19 , HIGH(2000)
	sts ICR1L , r18  
	sts ICR1H,  r19 
	jmp main_	
	
	
PWM_1000:
    
        ldi r18 , LOW(1999)
	ldi r19, HIGH(1999)
	sts OCR1Al , r18  // r4 = 0x80
	sts OCR1Ah,  r19 // r5 = 0x00 start from 50/100  
 
        ldi r18 , LOW(1000)
	ldi r19 , HIGH(1000)
	sts ICR1L , r18  
	sts ICR1H,  r19 
	jmp main_	
	
	

	







