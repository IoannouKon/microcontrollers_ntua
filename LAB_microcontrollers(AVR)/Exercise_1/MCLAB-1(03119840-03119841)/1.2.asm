;F0 = (A' * B' + B'*D )' 
;F1 = (A+C) * (B+D') 
   
.include "m328PBdef.inc"

.DEF A = r16
.DEF AN = r21 
.DEF B = r17
.DEF C = r18 
.DEF D = r19    ; Δήλωση καταχωρητών
.DEF BN = r22
.DEF CN = r23
.DEF DN = r24 
.DEF temp =r28
.DEF check =r20 
.DEF F0 = r30
.DEF F1 =r29
    
;.cseg
;.org 0 ; διεύθυνση εκκίνισης
.equ loops = 6

 
ldi check ,loops 

;Αρχικές τιμές 
ldi A,0x55
ldi B,0x43 
ldi C,0x22
ldi D,0x02

Loop: 
;Δημιουργώ τα συμληρώματα 
mov AN,A
com AN
mov BN,B
com BN
mov CN,C
com CN
mov DN,D
com DN 
   
;F0 = (A' * B' + B'*D )' 
clr F0
mov F0,AN 
and F0,BN
mov temp,BN 
and temp,D 
or F0,temp 
com F0
    
;F1 = (A+C) * (B+D') 
clr F1
mov F1,A
or F1,C 
mov temp,B
or temp,DN
and F1,temp 

DEC check
cpi check ,0    
breq EXIT   ; skip if check != 0 

    
      
subi A ,-0x02
subi B,-0x03
subi C,-0x04
subi D,-0x05
    
jmp Loop 
    
EXIT:
    
    
