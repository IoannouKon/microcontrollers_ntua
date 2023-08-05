.include "m328PBdef.inc"
  
    .equ FOSC_MHZ=16
    .equ DEL_mS = 50
    .equ DEL_NU=FOSC_MHZ * DEL_mS
    
  .org 0x0   
   rjmp reset 
  .org 0x2
   rjmp ISR0
      
 reset: 
 ldi r24 , (1 << ISC01) | (1 << ISC00) 
 sts EICRA , r24
 ldi r24, (1<< INT0)
 out EIMSK ,r24 
 ldi r24, (1<<INTF0)
 out EIFR, r24
 sei 
    
 
    
   ldi r24,LOW(RAMEND)
   out SPL ,r24
   ldi r25,HIGH(RAMEND)
   out SPH ,r25
   clr r26
   out DDRB, r26
   ser r26
   out DDRC ,r26
   loop1: 
    clr r26   
    
   loop2:
    out PORTC ,r26
    ldi r24,low(DEL_NU)
    ldi r25,high(DEL_NU)
    rcall delay_mS //
    
    inc r26
    
    cpi r26 ,32
    breq loop1
    rjmp loop2
    
    delay_mS:
      ldi r23,120
      nop
      nop
      nop
      
    loop_inn:
    dec r23
    nop
    brne loop_inn
    sbiw r24 ,1 
    brne delay_mS //
    ret 
        
////////////////////////////////////////// 
 ;ρουτίνα εξυπηρέτησης 

    ISR0: 
    push r25
    push r24
    in r24,SREG 
    push r24 
    
   Do_It:
   clr r18
   in r20 ,PINB  ;ανάγνωση αριθμού απο την θύρα B
   ;com r20
   clr r30 ; Εδώ θα κρατάμε το πλήθος των μπουτον της PortB που ειναι πατημένα 
   
   ldi r31, 8 //6
   ldi r28 , 1  
   jmp loopX
   
  L2: inc r30 
    nop
    nop
      dec r30 
    breq CON
    clr r18
   out PORTC ,r18;PC
   ldi r18 ,1 //
   
   LEDON:
    dec r30 
    breq CON
     lsl r18 
    ori  r18 , 1
    jmp LEDON 
    
   
   
  CON: 
   out PORTC,r18 
   ldi r24,low(DEL_NU)
   ldi r25,high(DEL_NU)
   rcall delay_mS
    
    pop r24
    out SREG ,r24
    pop r24
    pop r25
    sei
    reti 
    
    

   
   
   
   loopX: 
    mov r29 ,r20
    and  r29,r28 ;τσεκάρουμε ενα - ενα τα ψηφία απο LSB σε MSB  
    cpi r29 , 0  ; εξτάζουμε αν έχουμε μονάδα σε αυτή την θέση ή οχι 
   ; αν ισχύει το παραπάνω  r29==0  skip την επόμενη γραμμή
    breq L1 
    inc r30
    
   L1:  
    lsl r28 ; παμε μια θέση αριστερά την μονάδα στον r28
    dec r31 
    breq  L2 ; Αν τσεκάρεις με όλα τα buttons του portB βγες απο το loop
    jmp loopX
    
  

    
   
    
   


