///////////////////////////////////////////// lCD with extender 

/* Στην main 
      twi_init();
      PCA9555_0_write(REG_CONFIGURATION_0, 0x00); // for extender */

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


/*
initialize lcd
*/
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
{   
    int i=0;
     
  while (txt[i] != '\n') {
    lcd_data(txt[i]);
    i++;
  }
}


/* Extra command for LCD 

lcd_init();

lcd_command(0x01); // clear screen


 //For 2 lines 
lcd_command(0x28);  
lcd_command(0x01);
lcd_command(0b10000000);

 //print in first line (LCD_data)

  lcd_command(0b11000000);

 //print in second line (LCD_data) 

*/



///////////////////////////////// LCD - with PORTD 

// Στην main DDRD |= 0b11111111;  // output for LCD
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






