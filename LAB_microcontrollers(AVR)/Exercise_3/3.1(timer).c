#define F_CPU 16000000UL
#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>
int x;
int check;
int flag;


ISR(INT1_vect){
    while(PIND==11110111);
    EIFR=0x00000010;
    
     if(flag==0){
         PORTB=1;
         TCNT1=3036;
         TCCR1A=0;
         TCCR1B=5;
         flag=1;
         
     }
     else{
                  TCNT1=3036;
         PORTB=0xFF;
         _delay_ms(300);
         PORTB=1;

}    
}
ISR(TIMER1_OVF_vect){
        PORTB=0;
        flag=0;
    
}
int main(){
 DDRB=0xFF;
 DDRC=0x00;
 EICRA=(1<<ISC11)|(1<<ISC10);
 EIMSK=(1<<INT1);
 TIMSK1=(1<<TOIE1);
 sei();
 flag=0;

 while(1){
 check=1;
  while(check){
     x=PINC;
     x=x&32;
     if(x==0) check=0;
  }
 while((PINC&32)==0);
     if(flag==0){
         PORTB=1;
         TCNT1=3036;
         TCCR1A=0;
         TCCR1B=5;
         flag=1;
     }
     else{
         TCNT1=3036;
         PORTB=0xFF;
         _delay_ms(500);
         PORTB=1;
     }
     
 }
}
