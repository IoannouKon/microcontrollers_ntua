
.include "m328PBdef.inc"

reset:
  ldi r24,low(RAMEND)
  out SPL,r24
  ldi r24,high(RAMEND)
  out SPH , r24
    
 .equ msec=    845   // Εδώ δίνουμε πόσα ms θέλουμε να διαρκεί το πρόγραμμα μας 
   
main:
     rcall waitx
    rjmp main 
    
 
    
   
  waitx: 
    nop
    ldi r24,low(msec)  //1 usec
    ldi r25,high(msec) //1 usec
    sbiw r24,1      //2 usec
    breq LOOP     // 1 or 2  , Αν θέλουμε 1 ms delay πάμε σε συγκεκριμένη περίπτωση 
    jmp wait11      //2 usec  
    
     
loop2:  // επαναλαμβάνεται όσα ms ζητάμε μειον μια φορά 
rcall wait1m       //3 usec +....
sbiw r24,1      //2 usec
brne loop2 //1 or 2 
 ret // 4 usec 

wait4:
   ret
    
wait1m:  // for loop2 delay 
    ldi r26,98
  loop1:
   rcall wait4
    dec r26
    brne loop1
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
ret

; 10usec + 4usec*loop + 1 msec * loop - >  10 usec +1msec*loop  αλλάζοντας την wait 1m να κανει 996 usec 
wait11:     //987 usec  ... for DC delay 
    ldi r26,98
  loop11:
   rcall wait4
    dec r26
    brne loop11
 jmp loop2
 

LOOP: //for 1 ms only
    wait:  // for loop2 delay 
    ldi r26,98
  loop12: // 
   rcall wait4
    dec r26
    brne loop12
 nop
 nop
 nop
 nop
 nop
 nop
 nop
jmp main
 

