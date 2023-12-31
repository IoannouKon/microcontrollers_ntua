
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

////////////////////////////////////////

///////////////////////////////////keyboard

/* Για την main 
twi_init();
 PCA9555_0_write(REG_CONFIGURATION_1, 0xF0); //11110000 IO1_0-IO1_3 output  and IO1_4-IO1_7 input
*/

int *scan_row(int i) //NEW EDITION
{ 
 // i ??????? ??? ??? scan_keypad ??? ?? ??????? ???? ?????? ?????????? , i = 0,1,2,3   
  static int row[4]  = {0,0,0,0};
  uint8_t y ,z;
  int x; 
  //x =pow(2,i);
  row[1]=row[2]=row[3]=row[0]=0;
                            
PCA9555_0_write(REG_OUTPUT_1, i);  // ?? ??? 0x0E =1110  ???? ?????? 0 ?? IO1_0 (?????????? ??? ????? ??????)
y=PCA9555_0_read(REG_INPUT_1); //??????? ?? IO1 
y=y&0xF0; //x&11110000 => ????????? ?? 4 MSB 
y=y>>4;   // ?? ???????? ????? ???? ?? ?????? LSB 
//y = 0000y3y2y1y0
  x=0b00000001;
  
  for (int j=0;j<4;j++) {   
      z=x&y ; 
      x=x*2;
      if(z== 0b000000000)   {  row[j]=1; } 
    }
  
  
return row;
} 




int *scan_keypad()
 { int z = 0 ; 
  static int  keypad[16] ;
   int *temp;
    
    int i ;
	for(int x=0;x<4;x++) 
  {   
        
        switch(x) {
        case 0:
            i=0b1110;
            break;
        case 1: 
             i=0b1101;
            break;  
        case 2:
            i=0b1011;
            break; 
        case 3: 
            i=0b0111;
            break;   
        }
        
       //i = pow(2,x) ;
       temp=scan_row(i);
       
        for(int j=0;j<4;j++){ 
         keypad[z+j]= *(temp+j);
  }
       z=z+4;
    }
   

return keypad;
}

int *scan_keypad_rising_edge()
{ 
   static int temp1[16] ;
   static int temp2[16] ;
   int *t;
   
    t = scan_keypad();
    for(int j=0;j<16;j++){ 
         temp1[j]= *(t+j);
    }
   _delay_ms(15);  // (10-20ms);
   
    t = scan_keypad();
    for(int j=0;j<16;j++){ 
         temp2[j]= *(t+j);
    }
   

for(int i =0 ;i<16;i++) { 
   if(temp1[i] != temp2[i]) temp1[i] = 0; 
   }
    

	return temp1;
}

const char keypad_to_ascii(int temp[])
{  
   static int j ;
   int flag=0;
   char str ;
   char keypad_layout[16]={ '*','0','#','D',      
                             '7','8','9','C', 
                            '4','5','6','B',                          
                            '1','2','3','A' }  ;
                            

   
   for( j=00;j<16;j++) {
      if(temp[j]==1) {
          flag=1;
       goto key_found;
    }
   }
    
key_found: 
       if(flag==1){           
       str = keypad_layout[j];
       return str;
       }
       else return ' ';
}
 
char  key_press() { // επιστρέφει τον χαρακήρα που πατήσαμε στο  πληκτρολόγιο αν δεν πατήσαμε κάποιον επιστρέφει τον κενό χαρακήρα 
   
   int keypad[16]; 
   int *temp;
   char key;

   temp = scan_keypad_rising_edge();
    for(int j=0;j<16;j++){ 
            keypad[j]= *(temp+j);
         }

     key = keypad_to_ascii(keypad);

   return key;
  
  }








 ///////////////////////////// 
  
     
     int main() {
            
      twi_init();
     // PCA9555_0_write(REG_CONFIGURATION_0, 0x00);   // for extedn 
      PCA9555_0_write(REG_CONFIGURATION_1, 0xF0); //11110000 IO1_0-IO1_3 output  and IO1_4-IO1_7 input
      
      
     // lcd_init();
      //lcd_command(0x01);
      DDRC |= 0xFF ; // PORTC as output
      PORTC =0x0;
      
         DDRB |= 0xFF ; // PORTB as output
         PORTB =0x0;
       int   y=0;
      while(1){
        /* char x = key_press();
         //a) 
          if(x=='#'){ 
              PORTC = 0x01;
              _delay_ms(1000);
               PORTC = 0x0;
          */
          
         //b)
               char x = key_press();
              // int y; // save ascii
              if(x != ' ') y = x ;//-48;
              
        //c)       
          //y =y7...y0     
               int z ;
               int counter = 0;
               for(int i =0 ; i < 7; i++) {
                   //z = 0x01; 
                   z = (0x01&y);
                   if(z==0x1) counter++ ;
                   y = y>>1;
               }
               
         //d)
               PORTB = counter ; 
               
          }
                 
                
                
                 
          
               
          }
          
            
          
          
     
    
         
       

        

        
   
   
   
   
 
