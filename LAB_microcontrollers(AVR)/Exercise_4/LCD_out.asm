.include "m328PBdef.inc"
 .equ PD3=3 
 .equ PD2=2
 
    write_2_nibbles:
push r24
; στέλνει τα 4 MSB
in r25 ,PIND
; διαβάζονται τα 4 LSB και τα ξαναστέλνουμε
andi r25 ,0x0f
; για να μην χαλάσουμε την όποια προηγούμενη κατάσταση
andi r24 ,0xf0
; απομονώνονται τα 4 MSB και
add r24 ,r25
; συνδυάζονται με τα προϋπάρχοντα 4 LSB
out PORTD ,r24
; και δίνονται στην έξοδο
sbi PORTD ,PD3
; δημιουργείται παλμός Enable στον ακροδέκτη PD3
cbi PORTD ,PD3
; PD3=1 και μετά PD3=0
pop r24
; στέλνει τα 4 LSB. Ανακτάται το byte.
swap r24
; εναλλάσσονται τα 4 MSB με τα 4 LSB
andi r24 ,0xf0
; που με την σειρά τους αποστέλλονται
add r24 ,r25
out PORTD ,r24
sbi PORTD ,PD3
; Νέος παλμός Enable
cbi PORTD ,PD3
ret
    
lcd_data:
sbi PORTD ,PD2
; επιλογή του καταχωρητή δεδομένων (PD2=1)
rcall write_2_nibbles
; αποστολή του byte
ldi r24 ,43
; αναμονή 43μsec μέχρι να ολοκληρωθεί η λήψη
ldi r25 ,0
; των δεδομένων από τον ελεγκτή της lcd
rcall wait_usec

    
lcd_command:
cbi PORTD ,PD2
; επιλογή του καταχωρητή εντολών (PD2=1)
rcall write_2_nibbles
; αποστολή της εντολής και αναμονή 39μsec
ldi r24 ,39
; για την ολοκλήρωση της εκτέλεσης της από τον ελεγκτή της lcd.
ldi r25 ,0
; ΣΗΜ.: υπάρχουν δύο εντολές, clear display και return home,
rcall wait_usec
; που απαιτούν σημαντικά μεγαλύτερο χρονικό διάστημα.
    
    
lcd_init:
ldi r24 ,40
; Όταν ο ελεγκτής της lcd τροφοδοτείται με
ldi r25 ,0
; ρεύμα εκτελεί την δική του αρχικοποίηση.
rcall wait_msec
; Αναμονή 40 msec μέχρι αυτή να ολοκληρωθεί.
ldi r24 ,0x30
; εντολή μετάβασης σε 8 bit mode
out PORTD ,r24
; επειδή δεν μπορούμε να είμαστε βέβαιοι
sbi PORTD ,PD3
; για τη διαμόρφωση εισόδου του ελεγκτή
cbi PORTD ,PD3
; της οθόνης, η εντολή αποστέλλεται δύο φορές
ldi r24 ,39
ldi r25 ,0
; εάν ο ελεγκτής της οθόνης βρίσκεται σε 8-bit mode
rcall wait_usec
; δεν θα συμβεί τίποτα, αλλά αν ο ελεγκτής έχει διαμόρφωση
; εισόδου 4 bit θα μεταβεί σε διαμόρφωση 8 bit
ldi r24 ,0x30
out PORTD ,r24
sbi PORTD ,PD3
cbi PORTD ,PD3
ldi r24 ,39
ldi r25 ,0
rcall wait_usec
ldi r24 ,0x20
; αλλαγή σε 4-bit mode
out PORTD ,r24
sbi PORTD ,PD3
cbi PORTD ,PD3
ldi r24 ,39
ldi r25 ,0
rcall wait_usec
ldi r24 ,0x28
; επιλογή χαρακτήρων μεγέθους 5x8 κουκίδων
rcall lcd_command
; και εμφάνιση δύο γραμμών στην οθόνη
ldi r24 ,0x0c
; ενεργοποίηση της οθόνης, απόκρυψη του κέρσορα
rcall lcd_command
ldi r24 ,0x01
; καθαρισμός της οθόνης
rcall lcd_command
ldi r24 ,low(1530)
ldi r25 ,high(1530)
rcall wait_usec
ldi r24 ,0x06
; ενεργοποίηση αυτόματης αύξησης κατά 1 της διεύθυνσης
rcall lcd_command
; που είναι αποθηκευμένη στον μετρητή διευθύνσεων και
; απενεργοποίηση της ολίσθησης ολόκληρης της οθόνης
ret
    
    
    
    
main:
ldi r24, low(RAMEND)
out SPL, r24
ldi r24, high(RAMEND)
out SPH, r24
; αρχικοποίηση stack pointer
ser r24
out DDRD, r24
; αρχικοποίηση PORTD που συνδέεται η οθόνη, ως έξοδος
clr r24
    
main1:
rcall lcd_init
; αρχικοποίηση οθόνης
ldi r24, low(2)
ldi r25, high(2)
; Αναμονή 2 msec
rcall wait_msec
ldi r24, 'A'
rcall lcd_data
; αποστολή ενός byte δεδομένων στον ελεγκτή της οθόνης lcd
ldi r24, low(2000)
ldi r25, high(2000)
; Αναμονή 2 sec
rcall wait_msec
jmp main1
    
    
wait_msec:     	
   	rcall wait_usec       			                     
   	sbiw r24 , 1          	
   	brne wait_msec        	
   	ret			

wait_usec:
	//rcall light_on   
	sbiw r24 ,1      		
	nop           		
	nop          		
	nop           		
	nop           		
	brne wait_usec		
    ret             		

    
  
