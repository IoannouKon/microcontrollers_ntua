.include "m16def.inc"
.DEF A = r16
.DEF B = r17
.DEF C = r18 
.DEF D = r19    ; Δήλωση καταχωρητών
.DEF BN = r22
.DEF CN = r23
.DEF DN = r24 
.DEF temp =r21
.DEF F0 = r30

.cseg
.org 0 

start: 
    clr temp 
	out DDRB ,temp ; Θύρα Β ως είσοδος
	ser temp      
	out DDRA,temp   ; θύρα Α ως έξοδος

again:
clr F0
in temp ,PINB ;Ανάγνωση ακροδεκτων  PORTB
mov A,temp ; A στο LSB του καταχωρητή Α 
lsr temp  
mov B ,temp ; B στο LSB του καταχωρητή Β 
mov BN ,temp
com BN  ; BN συμπήρωμα του Β
lsr temp    
mov C,temp  ; C στο LSB του καταχωρητή C
mov CN,temp 
com CN    ; CN Συμπλήρωμα του C
lsr temp
mov D ,temp ;D στο LSB του καταχωρητή D
mov DN ,temp
com DN ;DN συμπλήρωμα του D


mov F0,A    ; F0=A
and F0,B    ; F0 = A*B  
and F0,C    ;F0=A*B*C
and BN,CN   ; BN = BN*CN
and BN,DN   ; BN = BN*CN*DN
or F0,BN    ; F0 = A*B*C + BN*CN*DN


or  A , B   ;A = A + B
or A ,C   ; A = A+B+C
and A , D   ; A=(A+B+C)*D

lsl A
andi A,0x02     ;απομόνωση του  2o LSB

andi F0,0x01     ;απομόνωση του 1ou LSB
;andi A,0x02     ;απομόνωση του LSB

or F0,A 
out PORTA,F0  ;έξοδος αποτελέσματος

rjmp again