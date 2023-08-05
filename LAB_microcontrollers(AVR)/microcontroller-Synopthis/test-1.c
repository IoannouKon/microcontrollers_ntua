
#define F_CPU 16000000UL
#include "avr/io.h"
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
 
char temp_buffer[10], pres_buffer[10]; 
int flag ; //  flag : OK=0 , CN =1 ,CT =2 ,CP=3
/////////////////////////////////////////////////TWI
#define PCA9555_0_ADDRESS 0x40 //A0=A1=A2=0 by hardware
#define TWI_READ 1 // reading from twi device
#define TWI_WRITE 0 // writing to twi device
#define SCL_CLOCK 100000L // twi clock in Hz
//Fscl=Fcpu/(16+2*TWBR0_VALUE*PRESCALER_VALUE)
#define TWBR0_VALUE ((F_CPU/SCL_CLOCK)-16)/2
// PCA9555 REGISTERS
typedef enum {
 REG_INPUT_0 = 0,
 REG_INPUT_1 = 1,
 REG_OUTPUT_0 = 2,
 REG_OUTPUT_1 = 3,
 REG_POLARITY_INV_0 = 4,
 REG_POLARITY_INV_1 = 5,
 REG_CONFIGURATION_0 = 6,
 REG_CONFIGURATION_1 = 7,
} PCA9555_REGISTERS;
//----------- Master Transmitter/Receiver -------------------
#define TW_START 0x08
#define TW_REP_START 0x10
//---------------- Master Transmitter ----------------------
#define TW_MT_SLA_ACK 0x18
#define TW_MT_SLA_NACK 0x20
#define TW_MT_DATA_ACK 0x28
//---------------- Master Receiver ----------------
#define TW_MR_SLA_ACK 0x40
#define TW_MR_SLA_NACK 0x48
#define TW_MR_DATA_NACK 0x58
#define TW_STATUS_MASK 0b11111000
#define TW_STATUS (TWSR0 & TW_STATUS_MASK)
//initialize TWI clock
void twi_init(void)
{
 TWSR0 = 0; // PRESCALER_VALUE=1
 TWBR0 = TWBR0_VALUE; // SCL_CLOCK 100KHz
}
// Read one byte from the twi device ( request more data from device)
unsigned char twi_readAck(void)
{
TWCR0 = (1<<TWINT) | (1<<TWEN) | (1<<TWEA);
while(!(TWCR0 & (1<<TWINT)));
 return TWDR0;
}
// Issues a start condition and sends address and transfer direction.
// return 0 = device accessible, 1= failed to access device
unsigned char twi_start(unsigned char address)
{
 uint8_t twi_status;
// send START condition
TWCR0 = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
// wait until transmission completed
while(!(TWCR0 & (1<<TWINT)));
// check value of TWI Status Register.
twi_status = TW_STATUS & 0xF8;
if ( (twi_status != TW_START) && (twi_status != TW_REP_START)) return 1;
// send device address
TWDR0 = address;
TWCR0 = (1<<TWINT) | (1<<TWEN);
// wail until transmission completed and ACK/NACK has been received
while(!(TWCR0 & (1<<TWINT)));
// check value of TWI Status Register.
twi_status = TW_STATUS & 0xF8;
if ( (twi_status != TW_MT_SLA_ACK) && (twi_status != TW_MR_SLA_ACK) )
 {
 return 1;
 }
return 0;
}
// Send start condition, address, transfer direction.
// Use ack polling to wait until device is ready
void twi_start_wait(unsigned char address)
{
 uint8_t twi_status;
 while ( 1 )
 {
// send START condition
TWCR0 = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);

 // wait until transmission completed
 while(!(TWCR0 & (1<<TWINT)));

 // check value of TWI Status Register.
 twi_status = TW_STATUS & 0xF8;
 if ( (twi_status != TW_START) && (twi_status != TW_REP_START)) continue;

 // send device address
 TWDR0 = address;
 TWCR0 = (1<<TWINT) | (1<<TWEN);

 // wail until transmission completed
 while(!(TWCR0 & (1<<TWINT)));

 // check value of TWI Status Register.
 twi_status = TW_STATUS & 0xF8;
 if ( (twi_status == TW_MT_SLA_NACK )||(twi_status ==TW_MR_DATA_NACK) )
 {
 /* device busy, send stop condition to terminate write operation */
 TWCR0 = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);

 // wait until stop condition is executed and bus released
 while(TWCR0 & (1<<TWSTO));

 continue;
 }
break;
 }
}
// Send one byte to twi device, Return 0 if write successful or 1 if write failed
unsigned char twi_write( unsigned char data )
{
// send data to the previously addressed device
TWDR0 = data;
TWCR0 = (1<<TWINT) | (1<<TWEN);
// wait until transmission completed
while(!(TWCR0 & (1<<TWINT)));
if( (TW_STATUS & 0xF8) != TW_MT_DATA_ACK) return 1;
return 0;
}
// Send repeated start condition, address, transfer direction
//Return: 0 device accessible
// 1 failed to access device
unsigned char twi_rep_start(unsigned char address)
{
 return twi_start( address );
}
// Terminates the data transfer and releases the twi bus
void twi_stop(void)
{
 // send stop condition
TWCR0 = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
// wait until stop condition is executed and bus released
while(TWCR0 & (1<<TWSTO));
}
void PCA9555_0_write(PCA9555_REGISTERS reg, uint8_t value)
{
 twi_start_wait(PCA9555_0_ADDRESS + TWI_WRITE);
 twi_write(reg);
 twi_write(value);
 twi_stop();
}

unsigned char twi_readNak(void) 
{
    TWCR0 = (1<<TWINT)|(1<<TWEN);
    while(!(TWCR0&(1<<TWINT)));
    return TWDR0;
}
uint8_t PCA9555_0_read(PCA9555_REGISTERS reg)
{
uint8_t ret_val;

 twi_start_wait(PCA9555_0_ADDRESS + TWI_WRITE);
 twi_write(reg);
 twi_rep_start(PCA9555_0_ADDRESS + TWI_READ);
 ret_val = twi_readNak();
 twi_stop();

 return ret_val;
}
////////////////////////////////////////////////////// LCD

void write_2_nibbles(unsigned char a){
    unsigned char b;
    b=a;
    unsigned char c;
    c=PCA9555_0_read(REG_OUTPUT_0);
    c=c & (0x0F);
    a=a & (0xF0);
    a=a+c;
    PCA9555_0_write(REG_OUTPUT_0, a);//PORTD = a;
    a=(a | 0b00001000);
    PCA9555_0_write(REG_OUTPUT_0,a );//PORTD=PORTD|8;
    a=(a & 0b11110111);
    PCA9555_0_write(REG_OUTPUT_0, a);    //PORTD=PORTD & (0b11110111);
    a=b;
    a=a<<4;
    a=a & (0xF0);
    a=a+c;
    PCA9555_0_write(REG_OUTPUT_0, a);//PORTD=a;
    a=(a | 0b00001000);
    PCA9555_0_write(REG_OUTPUT_0,a );//PORTD=PORTD|8;
    a=(a & 0b11110111);
    PCA9555_0_write(REG_OUTPUT_0, a);    //PORTD=PORTD & (0b11110111);
    //PORTD=PORTD|8;
    //PORTD=PORTD&(0b11110111);
}

void lcd_data(unsigned char x){
    unsigned char y=PCA9555_0_read(REG_OUTPUT_0);
    PCA9555_0_write(REG_OUTPUT_0, (y | 0b00000100));//PORTD=PORTD|4;
    write_2_nibbles(x);
    _delay_us(100);
}

void lcd_command(unsigned char x){
    
    unsigned char y= PCA9555_0_read(REG_OUTPUT_0);
    PCA9555_0_write(REG_OUTPUT_0, (y & 0b11111011));//PORTD = PORTD & (0b11111011);
    write_2_nibbles(x);
    _delay_us(100);
}

void lcd_init(void){
    unsigned char x;
    _delay_ms(100);
    PCA9555_0_write(REG_OUTPUT_0, 0x30);    //    PORTD=0x30;
    PCA9555_0_write(REG_OUTPUT_0, 0x38);    //    PORTD=PORTD|8; 
    PCA9555_0_write(REG_OUTPUT_0, 0x30);    //    PORTD=PORTD&(0b11110111);
    _delay_us(100);
    PCA9555_0_write(REG_OUTPUT_0, 0x30);    //    PORTD=0x30;
    PCA9555_0_write(REG_OUTPUT_0, 0x38);    //    PORTD=PORTD|8; 
    PCA9555_0_write(REG_OUTPUT_0, 0x30);//    PORTD=0x30;

    _delay_us(100);
    PCA9555_0_write(REG_OUTPUT_0, 0x20);    //    PORTD=0x20;
    PCA9555_0_write(REG_OUTPUT_0, 0x28);    //    PORTD=PORTD|8; 
    PCA9555_0_write(REG_OUTPUT_0, 0x20);
    _delay_us(100);
    x=0x28;
    lcd_command(x);
    x=0x0c;
    lcd_command(x);
    x=0x01;
    lcd_command(x);
    _delay_us(4000);
    x=0x06;
    lcd_command(x);
}

void display_string(const char *txt) // display string on LCD
{   int i=0;
     
  while (txt[i] != '\n') {
    lcd_data(txt[i]);
    i++;
  }
}







 ///////////////////////////// 
  
     
     int main() {
            
      twi_init();
      PCA9555_0_write(REG_CONFIGURATION_0, 0x00); // for extedn 
       unsigned char F0,F1,temp1,A,B,C,D;
      
      lcd_init();
      //lcd_command(0x01);
      DDRB |= 0x0 ; // PORTB as input
     temp1=PINB;
     temp1=~temp1;
     temp1=temp1&0x0F //00001111
     A=temp1&0x01;  
     B=temp1&0x02;
     C=temp1&0x04;
     D=temp1&0x08;
     B=B>>1;
     C=C>>2;
     D=D>>3;
     
     //F0 = 
     //F1 = 
     int array[4] = {A,B,C,D};
     
     for(int i =0;i<3;i++) {
     if(array[i]==1) lcd_data('1');
     else lcd_data('0');
     }
     
     _delay_ms(10);
     
     /* 
      if(F0==1) lcd_data('1');
      else lcd_data('0');
    
     
      if(F1==1) lcd_data('1');
      else lcd_data('0');
      
     */
     
         }
       

        

        
   
   
   
   
 
