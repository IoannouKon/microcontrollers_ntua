
#define F_CPU 16000000UL
#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>
#include <stdio.h>
#include <math.h>
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
unsigned char twi_readNak(void)
{
TWCR0 = (1<<TWINT) | (1<<TWEN) | (1<<TWEN);
while(!(TWCR0 & (1<<TWINT)));
 return TWDR0;
}
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


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
 


/*
int main()
{ 
    DDRD |= 0b11111111;  // output for LCD
    lcd_init();
    
int keypad[16]; 
int *temp;
char key;

    
    
twi_init();
PCA9555_0_write(REG_CONFIGURATION_1, 0xF0); //11110000 ???? IO1_0-IO1_3 output  and IO1_4-IO1_7 input

   
    
while(1){
  
temp = scan_keypad_rising_edge();
   
      for(int j=0;j<16;j++){ 
         keypad[j]= *(temp+j); } 
    

       key = keypad_to_ascii(keypad); 

        lcd_command(0x01);
       _delay_ms(3);
       lcd_data(key); 
       _delay_ms(500);  
          
}
return 0;
 }
  
*/




char  take_number() { 
   
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


int main()
{ 
DDRB = 0xFF; // B as output 

twi_init();
PCA9555_0_write(REG_CONFIGURATION_1, 0xF0); //11110000 θέτω IO1_0-IO1_3 output  and IO1_4-IO1_7 input

char key1 ,key2,y1,y2;


  
while(1){
    
  key1=key2=' ';

 while(key1==' ') {
  key1 = take_number();
  }
  y1=key1;
  while(y1!=' ')
  {
   y1 = take_number();
  }

  while(key2==' ') {
  key2 = take_number();
  }
  y2 = key2;
  while(y2!=' ')
  {
   y2 = take_number();
  }
  
  if(key1=='6' && key2 == '4') {
    PORTB=0b00111111;
     _delay_ms(4000); 
     PORTB = 0x0;
     _delay_ms(1000); 
     }

  else { //blink();

    for (int x=0;x<10;x++ ) {
    PORTB=0b00111111;
     _delay_ms(250); 
     PORTB = 0x0;
     _delay_ms(250);
    }
    }
  
  }
return 0;
 }

