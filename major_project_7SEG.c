void sevensegmodule(int number){
  
    if (number >= 0 && number <= 9) {

        unsigned char SegPat[10] = {
        0x3F,         // pattern for 0
        0x06,         // pattern for 1
        0x5B,         // pattern for 2
        0x4F,         // pattern for 3
        0x66,         // pattern for 4
        0x6D,         // pattern for 5
        0x7D,         // pattern for 6
        0x07,         // pattern for 7
        0x7F,         // pattern for 8
        0x6F          // pattern for 9
        };       
        
        char digit[4] = {
        0xFE,         //enable first seven seg
        0xFD,         //enable second seven seg
        0xFB,         //enable third seven seg
        0xF7          //enable fourth seven seg
        };
        
        PORTB = 0;
        DDRB  = 0xFF;  //enable 7-seg
        DDRP  = 0x3F;  //Select which digit will be used
        PTP   = 0x07;
        
        
        PORTB = 0; //sets 7-seg low
        PTP = 0x07;  //turn the secod 7-seg on
        PORTB = SegPat[number];
    }
    
    return;   
}
