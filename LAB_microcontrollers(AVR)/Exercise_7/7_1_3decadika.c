#define F_CPU 16000000UL
#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int val ;

void write_2_nibbles(char x)
{
	char y=PIND & 0x0f;
	char x1=x & 0xf0;
	x1=x1+y;
	PORTD=x1;
	PORTD=PORTD | (1<<PD3);
	PORTD=PORTD & (0<<PD3);
	x=x<<4 | x>>4;
	x=x & 0xf0;
	PORTD=x+y;
	PORTD=PORTD | (1<<PD3);
	PORTD=PORTD & (0<<PD3);
}

void lcd_data(char x)
{
	PORTD=PORTD | (1<<PD2);
	write_2_nibbles(x);
	_delay_us(40);
}

void lcd_command(char x)
{
	PORTD=PORTD | (0<<PD2);
	write_2_nibbles(x);
	_delay_us(40);
}

void lcd_init (void)
{
	_delay_ms(40);
	PORTD=0x30;
	PORTD=PORTD | (1<<PD3);
	PORTD=PORTD & (0<<PD3);
	_delay_us(38);
	PORTD=0x30;
	PORTD=PORTD | (1<<PD3);
	PORTD=PORTD & (0<<PD3);
	_delay_us(38);
	PORTD=0x20;
	PORTD=PORTD | (1<<PD3);
	PORTD=PORTD & (0<<PD3);
	_delay_us(38);
	lcd_command(0x28);
	lcd_command(0x0c);
	lcd_command(0x01);
	_delay_ms(500);
	lcd_command(0x06);
}


///////////////////////////////////////////////////////////////////////////////


void LCD_print( float   x ) {
    
  
    
  if(x==0x800) { 
      lcd_command(0x01);
    _delay_ms(3);
      
      const char *txt= "NO Device";
      while (*txt)
	{
		lcd_data(*txt++); 
	}
     return 0; 
  }
  
  

 int temp;  
 temp = abs(x) ;
 //temp = 100*temp;
int  d1,d2,d3,d4,d5,d6;

d1=temp/100000;
temp=temp-d1*100000;
d2=temp/10000;
temp=temp-d2*10000;
d3=temp/1000;
temp=temp-d3*1000;
d4=temp/100;
temp=temp-d4*100;
d5=temp/10;
temp=temp-d5*10;
d6=temp;




unsigned  char ch1,ch2,ch3,ch4,ch5,ch6;
ch1 = d1 + '0';
ch2 = d2 + '0';
ch3 = d3 + '0';
ch4 = d4 + '0';
ch5 = d5 + '0';
ch6 = d6 + '0';

    lcd_command(0x01);
    _delay_ms(3);
    
    if (x<0) lcd_data('-');
    if (x>0) lcd_data('+');
      _delay_ms(10);
    lcd_data(ch1);
     _delay_ms(10);
    lcd_data(ch2);
     _delay_ms(10);
    lcd_data(ch3); 
    _delay_ms(10);
    lcd_data('.'); 
      _delay_ms(10);
    lcd_data(ch4);
     _delay_ms(10);
    lcd_data(ch5);
     _delay_ms(10);
    lcd_data(ch6); 
    _delay_ms(10);
   
} 
/////////////////////////////////////////////



 //int PD4=0b00010000;

 int one_wire_reset() {
     DDRD = 0b0;
     PORTD= 0b0;
    uint8_t sav;
   	DDRD|= 0b00010000;
  	PORTD&=0b11101111;
    _delay_us(480);
    DDRD&= 0b11101111;
    PORTD&=0b11101111;
    _delay_us(100);
    sav = PIND;
   _delay_us(380);
   sav = sav&0b00010000;
   sav = sav>>4;
   if(sav==0) return 1;  //if a connected devise is detected
   if(sav==1) return 0;  // if not
      }
      
 int  one_wire_receive_bit() { //read one bit from DS1820
     uint8_t x;
     DDRD|= 0b00010000;
     PORTD&= 0b11101111;
     _delay_us(2);
     DDRD&= 0b11101111;
     PORTD&=0b11101111;
     _delay_us(10);
    
     x=PIND;
     x=x&0b000010000;
     x = x>>4;
     _delay_us(49);
     return x;

   }
 
 void one_wire_trasnmit_bit(uint8_t x){
   
    DDRD|= 0b00010000;
    PORTD&=0b11101111;
   _delay_us(2);
   x=(x&(0b001));
   if(x==0) PORTD &=0b11101111;
   else{  PORTD |=0b00010000;}
      _delay_us(58);
      DDRD&= 0b11101111;
      PORTD&=0b11101111;
      _delay_us(1);  
   }
 
   
   uint8_t one_wire_receive_byte() { //read a byte from DS1820
     uint8_t x;
     uint8_t y = 0;
     
     for(int i =0 ;i<8;i++){
       x= one_wire_receive_bit();
       y = y>>1;
       if(x==0) y=x|y;    
       else {  y=0x080|y; }
     }
       return y;
        } 
   

void one_wire_trasnmit_byte(uint8_t x) {  //egrafi enos byte ston DS1820
      uint8_t y;
      for(int i =0 ;i<8;i++){ 
        if( (x&(0b01)) == 0) y=0b0;
        else y = 0b01;
        one_wire_trasnmit_bit(y);
        x= x>>1;
        }
        return 0;
    }    
         



          
   float find_temp(){
     
    int number=0;   
    int test = one_wire_reset();
    if (test == 0 ) return 0x800;
    
    
    one_wire_trasnmit_byte(0xCC);
    one_wire_trasnmit_byte(0x44);
   // printf("temperature measurement begins");
    
   do{test = one_wire_receive_bit() ;} 
    while(test==0) ;
   // printf("temperature measurement stops");
    
    one_wire_reset();
    one_wire_trasnmit_byte(0xCC);
    one_wire_trasnmit_byte(0xBE);
    uint8_t r25,r24; //r25::r24
    
    
   r24 =one_wire_receive_byte() ; //read first LSB and then MSB bits 
   r25 =one_wire_receive_byte() ;
  
    //r24=0xFF;
    //r25 =0xFF;
    uint16_t temp;
    
    temp = r25; 
    temp = temp<<8;
    temp = temp|r24;
    //temp=temp&0b0000011111111111;
    
    float x ;
    //r25 = r25>>8;
    r25 =r25&0b10000000;
    
    if(r25==0b0) {  x=(temp*(62.5));  return x; }
    
    if(r25==0b10000000) { 
        temp =~temp; 
        temp = temp+ 0b01;
        x = - (temp*(62.5));
        return x; }
    
   }  
    
    
   
   int main() {
       //lcd_init();
       
    while(1) {   
        int x =  find_temp();       
       DDRD |= 0b11111111;  // output for LCD
       PORTD=0;
       lcd_init();
      lcd_command(0x01);
      _delay_ms(30);
    
      
         LCD_print(x) ; 
        _delay_ms(1000);
       }
   }

        
   
   
   
   
 
