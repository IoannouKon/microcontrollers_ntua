/* Στην main βάζουμε 
 DDRC |= 0b00000000;  // input for ADC 
 ADCSRA = 0b10000111;  
 ADMUX  = 0b01000010; //REFS[2,1] = 01 -. Vref =5V 
                      //ADLAR = 0 ->δεξιά προσαρμοσμένο 
                      // MUX[3:0] -> επιλογή ADC εισόδου  (δες πινακάκι) 
 */

///////////////////////ADC απο την άσκηση 8

void ADC_check() {

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
      

      if( (value > 1200 || value < 400) && flag !=1 )  flag=3;
     
     
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


///////////////////////////////////////////////////////////////////////////
//πιο απλή έκδοση ΑDC μετατροπή (επιστρέφει την εξοδο του ADC *100) 

 unsigned int  ADC_check()
{

   int ADSC_check = 1;
    unsigned int value;
    unsigned  int temp1; 
   
	while(1)
    {
		if (ADSC_check)
		{
		
            //bit_is_set(ADCSRA, ADSC); // C3:: start converting voltage on A0
                        ADCSRA |= (1<<ADSC);
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
              
             ADSC_check = 1;
             return value;    
		  
		}
	}
   
}
 

