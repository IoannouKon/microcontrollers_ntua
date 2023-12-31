
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

///////////////////////////////////////USART

void usart_init(unsigned int ubrr) {
  UCSR0A = 0;
  UCSR0B = (1 << RXEN0) | (1 << TXEN0);
  UBRR0H = (unsigned char)(ubrr >> 8);
  UBRR0L = (unsigned char)ubrr;
  UCSR0C = (3 << UCSZ00);
  return;
}

void usart_transmit(uint8_t data) {
  while (!(UCSR0A & (1 << UDRE0)))
    ;
  UDR0 = data;
}

uint8_t usart_receive() {
  while (!(UCSR0A & (1 << RXC0)));
  return UDR0;
}
 
void usart_trasmit_string(const char *txt) 
{   
   while (*txt) {
   usart_transmit(*txt++);
  }
}

int  usart_receive_string() 
{  
    const char txt[20] ="\"Success\"\n" ;
    int x =10;
    char a ="Q";
    int i =0;
    while(a!='\n') { 
         a=usart_receive();
        if(a != txt[i]) x=  9;
        i++;
    }
   return x;
}

/*
char ok_buffer[100];
void usart_receive_string2() 
{  
    char a ='Q';
    int i=0;
    while(a=!'\n'){
         a=usart_receive();
         ok_buffer[i]=a;
         i++;
    }
}
*/

int  usart_receive_string3() 
{  
    const char txt[20] ="200 OK\n" ;
    int x =10;
    char a ="Q";
    int i =0;
    while(a!='\n') { 
         a=usart_receive();
        if(a != txt[i]) x=  9;
        i++;
    }
   return x;
}




//////////////////////////////////////////////////Temp
///////////////////////////////////////I2C
int one_wire_reset() {
  DDRD = 0b0;
  PORTD = 0b0;
  uint8_t sav;
  DDRD |= 0b00010000;
  PORTD &= 0b11101111;
  _delay_us(480);
  DDRD &= 0b11101111;
  PORTD &= 0b11101111;
  _delay_us(100);
  sav = PIND;
  _delay_us(380);
  sav = sav & 0b00010000;
  sav = sav >> 4;
  if (sav == 0)
    return 1; // if a connected devise is detected
  if (sav == 1)
    return 0; // if not
}

int one_wire_receive_bit() { // read one bit from DS1820
  uint8_t x;
  DDRD |= 0b00010000;
  PORTD &= 0b11101111;
  _delay_us(2);
  DDRD &= 0b11101111;
  PORTD &= 0b11101111;
  _delay_us(10);

  x = PIND;
  x = x & 0b000010000;
  x = x >> 4;
  _delay_us(49);
  return x;
}

void one_wire_trasnmit_bit(uint8_t x) {

  DDRD |= 0b00010000;
  PORTD &= 0b11101111;
  _delay_us(2);
  x = (x & (0b001));
  if (x == 0)
    PORTD &= 0b11101111;
  else {
    PORTD |= 0b00010000;
  }
  _delay_us(58);
  DDRD &= 0b11101111;
  PORTD &= 0b11101111;
  _delay_us(1);
}

uint8_t one_wire_receive_byte() { // read a byte from DS1820
  uint8_t x;
  uint8_t y = 0;

  for (int i = 0; i < 8; i++) {
    x = one_wire_receive_bit();
    y = y >> 1;
    if (x == 0)
      y = x | y;
    else {
      y = 0x080 | y;
    }
  }
  return y;
}

void one_wire_trasnmit_byte(uint8_t x) { // egrafi enos byte ston DS1820
  uint8_t y;
  for (int i = 0; i < 8; i++) {
    if ((x & (0b01)) == 0)
      y = 0b0;
    else
      y = 0b01;
    one_wire_trasnmit_bit(y);
    x = x >> 1;
  }
  return 0;
}

float find_temp() {

  int number = 0;
  int test = one_wire_reset();
  if (test == 0)
    return 0x800;

  one_wire_trasnmit_byte(0xCC);
  one_wire_trasnmit_byte(0x44);
  // printf("temperature measurement begins");

  do {
    test = one_wire_receive_bit();
  } while (test == 0);
  // printf("temperature measurement stops");

  one_wire_reset();
  one_wire_trasnmit_byte(0xCC);
  one_wire_trasnmit_byte(0xBE);
  uint8_t r25, r24; // r25::r24

  r24 = one_wire_receive_byte(); // read first LSB and then MSB bits
  r25 = one_wire_receive_byte();

  // r24=0xFF;
  // r25 =0xFF;
  uint16_t temp;

  temp = r25;
  temp = temp << 8;
  temp = temp | r24;
  // temp=temp&0b0000011111111111;
 
  temp = temp ; // for 36
  
  float x;
  // r25 = r25>>8;
  r25 = r25 & 0b10000000;

  if (r25 == 0b0) {
    x = (temp * (62.5)) ;  //62.5
    return x;
  }

  if (r25 == 0b10000000) {
    temp = ~temp;
    temp = temp + 0b01;
    x = -(temp * (62.5));
    return x;
  }
}


void conv_temp(float x) {


     
  if (x == 0x800) {
    lcd_command(0x01);
    _delay_ms(3);

    const char *txt = "NO Device";
    while (*txt) {
      lcd_data(*txt++);
    }
    return 0;
  }
  
  float temp;
  temp = abs(x) ;
 // temp = 100*temp;
  
   temp = temp+14000; // for 36 
    if ((37000 < temp || temp < 34000) && flag != 1)  flag =2 ;
  
    
    
  int d1, d2, d3, d4, d5, d6;
  //temp+=14000;
  d1 = (temp / 100000);
  temp = temp - d1 * 100000;
  d2 = (temp / 10000);
  temp = temp - d2 * 10000;
  d3 = temp / 1000;
  temp = temp - d3 * 1000;
  d4 = temp / 100;
  temp = temp - d4 * 100;
  d5 = temp / 10;
  temp = temp - d5 * 10;
  d6 = temp;

  
  //d2=d2+1; 
  //d3=d3+4;
  
  
  
  
  unsigned char ch1, ch2, ch3, ch4, ch5, ch6;
  ch1 = d1 + '0';
  ch2 = d2 + '0';
  ch3 = d3 + '0';
  ch4 = d4 + '0';
  ch5 = d5 + '0';
  ch6 = d6 + '0';

  temp_buffer[0] =ch2;
  temp_buffer[1] =ch3;
  temp_buffer[2] ='.';
  temp_buffer[3] =ch4;
  temp_buffer[4] =ch5;
  temp_buffer[5] =ch6;
  
}
/////////////////////////////////////////////////////////

///////////////////////////////////////Keyboard

int *scan_row(int i) //NEW EDITION
{ 
  
  static int row[4]  = {0,0,0,0};
  uint8_t y ,z;
  int x; 
  //x =pow(2,i);
  row[1]=row[2]=row[3]=row[0]=0;
                            
PCA9555_0_write(REG_OUTPUT_1, i); 
y=PCA9555_0_read(REG_INPUT_1);  
y=y&0xF0;  
y=y>>4;   
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
                            

   
   for( j=0;j<16;j++) {
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
 

char key_press()
{  int keypad[16]; 
   int *temp;
   char key;

   temp = scan_keypad_rising_edge();
    for(int j=0;j<16;j++){ 
            keypad[j]= *(temp+j);
         }

     key = keypad_to_ascii(keypad);

   return key;
  
  }

void connect_esp() {
    
   

    int x,i;
        
 usart_trasmit_string("ESP:connect\n");

    
    x = usart_receive_string() ;
     
   if ( x==10)  {
    // usart_trasmit_string("1.Success ");
        i = 0;
       char ar[12] = "1.Success\n";
       while(ar[i]!= '\n') { 
           lcd_data(ar[i]) ;
           i++;}
      _delay_ms(2000);
   }
   
       
    if (  x!=10) { 
        //usart_trasmit_string(" 1.Fail "); 
        lcd_command(0x01);
       i = 0;
       char ar[12] = "1.Fail\n";
       while(ar[i]!= '\n') { 
           lcd_data(ar[i]) ;
           i++;}
        _delay_ms(2000);
        
   }
    
  usart_trasmit_string("ESP:url: \"http://192.168.1.250:5000/data\"\n");
  
  x = usart_receive_string() ;
  
  if (  x==10 )  {
        //usart_trasmit_string(" 2.Fail "); 
       lcd_command(0x01);
       i = 0;
       char ar[12] = "2.Success\n";
       while(ar[i]!= '\n') { 
           lcd_data(ar[i]) ;
           i++;}
       _delay_ms(2000);
  }

if ( x!=10 ) {
  // usart_trasmit_string(" 2.Success "); 
     lcd_command(0x01);
        i = 0;
       char ar[12] = "2.Fail\n";
       while(ar[i]!= '\n') { 
           lcd_data(ar[i]) ;
           i++;}
         _delay_ms(2000);
   _delay_ms(2000);
   }
  
}
/////////////////////////////////////////////

/////ADC
void pressure_check() {

  int ADSC_check = 1;
  unsigned int value;
  unsigned int temp1;

  while (1) {
    if (ADSC_check) {

      // bit_is_set(ADCSRA, ADSC); // C3:: start converting voltage on A0
      ADCSRA |= (1 << ADSC);
      ADSC_check = 0;
    }

    if (   ( ADCSRA & (1 << ADSC)) == 0 )  //when conv is over     //bit_is_clear(ADCSRA, ADSC) 
		 {      
                        value = (ADC*5)/1024;
                        value =100*value;
                        temp1  = (((ADC*5)%1024)*10)/1024;
                        value=value +(10*temp1);
                        temp1  = ((((ADC*5)%1024)*10)%1024)*10/1024;
                        value = value +temp1 ;
      
                        value = value * 4;
      

      if( (value > 1200|| value < 400) && flag !=1 )  flag=3;
     
     
      int d1, d2, d3, d4, d5, d6,temp;  
       temp=value;
       d1 = temp / 1000;
       temp = temp - d1 * 1000;
       d2 = temp / 100;
       temp = temp - d2 * 100;
       d3 = temp / 10;
       temp = temp - d3 * 10;
       d4 = temp / 1;
       temp = temp - d4 * 1;
  

  unsigned char ch1, ch2, ch3, ch4, ch5, ch6;
  ch1 = d1 + '0';
  ch2 = d2 + '0';
  ch3 = d3 + '0';
  ch4 = d4 + '0';
  ch5 = d5 + '0';
  ch6 = d6 + '0';
   
      pres_buffer[0]=ch1;
      pres_buffer[1]=ch2;
      pres_buffer[2]='.';
      pres_buffer[3]=ch3;
      pres_buffer[4]=ch4;
      pres_buffer[5]=ch5;
    
      ADSC_check = 1;
      return;
    }
  }
}

/////////////////////////////////////// status




/*
Manual
1. 4: NURSE CALL -> #: OK   (flag=1)
2. if CHECK TEMP > 12 || CHECK TEMP < 4 : CHECK PRESSURE (flag = 2)
3. if temp_buffer < 34 || temp_buffer > 37 : CHECK TEMP  (flag =3)
4. default status = OK  (flag =0)
*/

void find_status() {
    char key = key_press(); 
    if(( flag==1) &&(key=="#"))    { flag =0 ;   }
    if((key == "4") && (flag==0) )  { flag =1; }       
}

  char status_buffer[15];
  void find_and_print_status() { 
  // char status_buffer[15];
      
       if(flag ==0 ) {
     status_buffer[0]='O';
     status_buffer[1]='K';
     status_buffer[2]='\n';
    }
       
     if(flag==1) {   
      status_buffer[0]='N';
      status_buffer[1]='U';
      status_buffer[2]='R';
      status_buffer[3]='S';
      status_buffer[4]='E';
     // status_buffer[5]=' ';
      status_buffer[5]='C';
      status_buffer[6]='A';
      status_buffer[7]='L';
      status_buffer[8]='L';
      status_buffer[9]='\n'; 
     } 
       
     if(flag==2){
      status_buffer[0]='C' ;
      status_buffer[1]='H';
      status_buffer[2]='E';
      status_buffer[3]='C';
      status_buffer[4]='K';
     // status_buffer[5]=' ';
      status_buffer[5]='T';
      status_buffer[6]='E';
      status_buffer[7]='M';
      status_buffer[8]='P';
      status_buffer[9]='\n';
      }   
        
     if(flag==3){
      status_buffer[0]='C' ;
      status_buffer[1]='H';
      status_buffer[2]='E';
      status_buffer[3]='C';
      status_buffer[4]='K';
    //  status_buffer[5]=' ';
      status_buffer[5]='P';
      status_buffer[6]='R';
      status_buffer[7]='E';
      status_buffer[8]='S';
      status_buffer[9]='S';
      status_buffer[10]='U';
      status_buffer[11]='R';
      status_buffer[12]='E';
      status_buffer[13]='\n';
       }
    
    
       int i=0;
      while(status_buffer[i]!= '\n'){  
            lcd_data(status_buffer[i]);
            i++;
        }
        _delay_ms(500);    
  } 

  
  void trasnmit_payload() {
      int i;
      
     
      
      
//usart_trasmit_string("ESP:payload:[{\"name\": \"temperature\",\"value\": \"38\"},{\"name\": \"pressure\",\"value\": \"60.0\"},{\"name\": \"team\",\"value\": \"64\"},{\"name\": \"status\",\"value\": \"OK\"}]\n");
   
      // first tropos  
    usart_trasmit_string("ESP:payload:[{\"name\": \"temperature\",\"value\": ");

     usart_transmit('"');
      for(i=0;i<6;i++) {  usart_transmit(temp_buffer[i]) ;}
      usart_transmit('"');

     usart_trasmit_string("},{\"name\": \"pressure\",\"value\": ");

     usart_transmit('"');
     for(i=0;i<5;i++) { usart_transmit(pres_buffer[i]) ;}
     usart_transmit('"');

     usart_trasmit_string("},{\"name\": \"team\",\"value\": \"64\"},{\"name\": \"status\",\"value\": ");

     usart_transmit('"');

     i=0;
     while(status_buffer[i]!= '\n'){  usart_transmit(status_buffer[i]); i++;}

     usart_transmit('"');
     usart_trasmit_string("}]\n");
 
 
//////////////////////////for 3.Success
lcd_command(0x01);
int  x = usart_receive_string() ;

  if (  x==10 )  { 
       lcd_command(0x01);
       i = 0;
       char ar[12] = "3.Success\n";
       while(ar[i]!= '\n') { 
           lcd_data(ar[i]) ;
           i++;}
       _delay_ms(2000);
  }

 if ( x!=10 ) {
     lcd_command(0x01);
        i = 0;
       char ar[12] = "3.Fail\n";
       while(ar[i]!= '\n') { 
           lcd_data(ar[i]) ;
           i++;}
         _delay_ms(2000);
   _delay_ms(2000);
   }

////////////////////////for transmit 200-OK
 

  lcd_command(0x01);
  _delay_ms(50);
  usart_trasmit_string("ESP:transmit\n");
  char test[15];
  test[0]=usart_receive();

  int  j=0;
  while(test[j]!= '\n') {
       j++;
      test[j]=usart_receive();
  }
  
   _delay_ms(500);
 // lcd_command(0x01);
  for(int i =0; i<j;i++){ 
      lcd_data(test[i]);
  }
   
 _delay_ms(2000);

 /*  
  if (  x==10 )  { 
       lcd_command(0x01);
       i = 0;
       char ar[12] = "200 OK\n";
       while(ar[i]!= '\n') { 
           lcd_data(ar[i]) ;
           i++;}
       _delay_ms(2000);
  }
   */
  
 ///////////////////////////// 
  
      
  } 
     int main() {
            
      DDRC |= 0b00000000; // input for ADC       
      ADCSRA = 0b10000111;
      ADMUX = 0b01000000; //POT0
  
      twi_init();
      PCA9555_0_write(REG_CONFIGURATION_0, 0x00); // for extender
      PCA9555_0_write(REG_CONFIGURATION_1, 0xF0); //11110000  keyboard-> IO1_0-IO1_3 output  and IO1_4-IO1_7 input
      
      lcd_init();
      //lcd_command(0x01);
      int i ;
      flag = 0; //  flag : OK=0 , CN =1 ,CT =2 ,CP=3
      int prev_flag=0;
       usart_init(103);
     
      
       
       while(1){ 
            lcd_command(0x01);
            connect_esp();
         
          flag = 0;
          int x = find_temp(); // x = x + kati
         // x = x +14; 
          conv_temp(x);     // make temp_buffer
            
          pressure_check();  // make pres_buffer
         
         
           _delay_ms(1000);
       
          lcd_command(0x28);  //first line 
          lcd_command(0x01);
          lcd_command(0b10000000);
       
        for(i=0;i<6;i++) { lcd_data(temp_buffer[i]) ;} // pirnt on lCD temp
       
          lcd_data('C');
          lcd_data(' ');
          lcd_data(' ');
        
         for(i=0;i<5;i++) { lcd_data(pres_buffer[i]) ;}//print on LCD pressure
        
          lcd_data('c');
          lcd_data('m');
                  
      
          //prev_flag =1 -> stay in Nurce Call 
          //prev_flag =0 -> check states  for OK,Check Temp,Check Press
          //find_status();
          char key = key_press();
          if(key == '4') prev_flag = 1;  
          if(key=='#') prev_flag = 0;
          if (prev_flag == 1) flag =1;
                    
         lcd_command(0b11000000); // second line 
    
        find_and_print_status();     
           _delay_ms(1000);
          trasnmit_payload();
     
         }
       }

        

        
   
   
   
   
 
