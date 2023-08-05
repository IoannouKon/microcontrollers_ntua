/////////////////////////////PWM 

/* Στην main βάζουμε 

TCCR1A = (1<<WGM10) | (1<<COM1A1);  //TCCR1A:COMA[1-2],COMB[1-2],___,____,WGM[11-10]
                                    //WGM[11-10] αρχικοποιεί τον TMR1A σε n-bits (πχ εδω 8 bits)
TCCR1B = (1<<WGM12) | (1<<CS11);    //TTCR1B:WGM[13-12],CS[12-10]
                                    //Με τα CS[12-10] -> prescaler(N) (δες πινακάκι)
//Αλλαγή φωτεινότητας 
OCR1A =        //   dc = παίρνει τιμές απο 0 εως 255 


/////////////////////////////Fast PWM Mode 

TCCR1A = (1<<WGM11) | (1<<COM1A1);  //  WGM[10:11] σε πόσα bit αρχικοποιούμε τον TMR1A  


TCCR1B = (1<<WGM13) | (1<<WGM12) | (1<<CS11);  // CS[12-10] Prescaler (N)

ICR1 = 16000; //   ICR1=TOP   , fpwm = flck/(N*TOP)
OCR1A=8000;   //   dc = ICR1/OCR1A   
