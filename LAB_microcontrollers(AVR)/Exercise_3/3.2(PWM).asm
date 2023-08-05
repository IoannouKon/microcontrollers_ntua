.include "m328PBdef.inc"
.equ FOSC_MHZ=16
.equ DEL_MS=500
.equ DEL_NU=FOSC_MHZ*DEL_MS
	clr r4
	clr r5
	clr r30
	clr r31

.def temp = r20  
	ser temp   
	out DDRB, temp   // B output   
	clr temp  
	out DDRD, temp   // D input   
	
	ldi temp ,(1<<COM1A1) | (1<<WGM10)  // ldi temp,  (0<<COM1A1) | (1<<WGM01) | (1<<WGM00)
	sts TCCR1A, temp   
	ldi temp, (1<<CS11)|(1<WGM12)       // ldi temp, (0<<CS10)
	sts TCCR1B, temp
	
	
	ldi zl, low(Table*2+12)      // zl =r30   
	ldi zh, high(Table*2)      // zh = r31  
	
	lpm r4 , Z+  ; load lower byte of var into r4  
	lpm r5 , z   ; load upper byte of var into r5
	sts OCR1Al , r4  // r4 = 0x80
	sts OCR1Ah,  r5  // r5 = 0x00 start from 50/100  
	ldi temp, 0b01 //0b01000000
	out PORTB, temp	

main_:
	in temp, PIND
	com temp
	cpi temp, 2   // PD1
	breq add_  
	cpi temp, 4 // PD2
	breq sub_
	rjmp  exit_  
	
	sub_: 
         
	   
                
                ldi r26 , 0x0 
                add r26 ,r4 
		cpi r26 ,0x0005   // if is 2/100 stop dec dc
		breq exit_
		subi zl, 2
		lpm r4 , Z+; load lower byte of var into r4  
		
	ldi r26 , 0x0 
                add r26 ,r4 
		cpi r26 ,0x0 
		breq sub_   
		sts OCR1Al, r4
	       //sts OCR1Ah, r0
	    
		jmp exit_

		

	add_:  
                ldi r26 , 0x0 
                add r26 ,r4 
		cpi r26 ,0x00DC   // if is 98/100 stop inc  dc
		breq exit_
		adiw zl , 1
		lpm r4 , Z+  ; load lower byte of var into r4
		
		
		
	/*	ldi r26 , 0x0 
                add r26 ,r4 
		cpi r26 ,0x0 
		breq add_   */
		
		sts OCR1Al, r4
	       //sts OCR1Ah, r0
		jmp exit_

	exit_:

		STS 0x0089,r5		// Store direct to data space 
		STS 0x0088,r4
		ldi r24, low(DEL_NU) ;
		ldi r25, high(DEL_NU)
		rcall delay_ms
		rjmp main_



delay_mS:
	ldi r23, 249
loop_inn:
	dec r23
	nop
	brne loop_inn
	sbiw r24, 1
	brne delay_mS
	ret





Table: // 13 τιμές για το DC απο 2/100 εως 98/100 με βήμα 8/100 
.DW 0x0005,0x0016,0x0028,0x003A,0x004C
.DW 0x005F,0x0080,0x0085,0x0094,0x00A6
.DW 0x00B8,0x00CA,0x00DC







