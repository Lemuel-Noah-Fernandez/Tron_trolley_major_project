
#include "derivative.h"
#include <math.h> 
#include "servo.h"
#include "simple_serial.h"


#define ZERO_ELEVATION_DUTY 4600
#define ZERO_AZIMUTH_DUTY 2000



#define numnotes 31
#define numdurations 7
//define each of the periods used for the notes

#define rest 20
#define Eb3 9641
#define E3  9100
#define F3  8589
#define Gb3 8107
#define G3  7652
#define Ab3 7223
#define A3  6817
#define Bb3 6434
#define B3  6073
#define C4  5733
#define Db4 5411
#define D4  5107
#define Eb4 4820
#define E4  4550
#define F4  4294
#define Gb4 4053
#define G4  3826
#define Ab4 3611
#define A4  3408
#define Bb4 3611
#define B4  3036
#define C5  2866
#define Db5 2705
#define D5  2553
#define Eb5 2410
#define E5  2275
#define F5  2147
#define Gb5 2026
#define G5  1913
#define Ab5 1805
#define A5  1704


#define longnote  128      
#define dbreve    64
#define breve     32
#define semibreve 16
#define minim     8
#define crotchet  4
#define quaver    2
#define semiquav  1

int mii_song_notes[] = {16, 19, 23, 19, 16, 12, 0, 12, 0, 12, 0, 16, 19, 23, 0, 19, 16, 26, 25, 24};  //20
int mii_song_duration[] = {5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 3, 5, 5, 5, 6, 5, 5, 4, 5, 5};
int period;

unsigned int note[] = {rest,        //0
    Eb3,                            //1
    E3,                             //2
    F3,                             //3
    Gb3,                            //4
    G3,                             //5
    Ab3,                            //6
    A3,                             //7
    Bb3,                            //8
    B3,                             //9
    C4,                             //10
    Db4,                            //11
    D4,                             //12
    Eb4,                            //13
    E4,                             //14
    F4,                             //15
    Gb4,                            //16
    G4,                             //17
    Ab4,                            //18
    A4,                             //19
    Bb4,                            //20
    B4,                             //21
    C5,                             //22
    Db5,                            //23
    D5,                             //24
    Eb5,                            //25
    E5,                             //26
    F5,                             //27
    Gb5,                            //28
    G5,                             //29
    Ab5                             //30
    };

unsigned int duration[] =
{
    dbreve,
    breve,
    semibreve,
    minim,
    crotchet, 
    quaver,
    semiquav
};



void delay_ms(unsigned int time){
    int i;
    int j;
    for(i=0;i<time;i++)
      for(j=0;j<4000;j++);
}


void PWMinitialise(void){
    // set PP5 and PP7 for pwm output 
    PWMCLK = 0; // select clock A
    PWMPOL = 0xA0; // PWM5 and PWM7 output start high
    PWMCTL = 0xC0; // 16-bit PWM: use PWM4 and PWM5 for PWM5, PWM6 and PWM7 for PWM7
    PWMCAE = 0; // Center aligned
    PWMPRCLK = 0x33; // PWM Clock prescaler to 8 

    // set the PWM period appropriate for servos
    PWMPER45 = 0xEA6A;
    PWMPER67 = 0xEA6A;

    // set the initial duty cycle for both servos
    PWMDTY45 = ZERO_ELEVATION_DUTY;
    PWMDTY67 = ZERO_AZIMUTH_DUTY;
    
    PWME  |= 0xFF;      // enable PWM0
}

void setServoPose(int angle, int elevation){  

    PWMDTY45 = (int)(ZERO_ELEVATION_DUTY + elevation);  // Sets elevation to duty cycle using PWM 45
    PWMDTY67 = (int)(ZERO_AZIMUTH_DUTY + 30*angle);   // Sets azimuth to duty cycle using PWM 67
}


void play_note(int note_num, int duration_num) {
  
  int time = duration[duration_num]*125;
  period = note[note_num];
  
  //toggle on successful output compare
	TCTL1 = 0x04;
  
  //enable timer interrupts
  TIE = 0x20;
  
  TIE_C6I = 1;   // enable interrupt for channel 6
  
  //set first output compare to happen 
	TC6 = TCNT + period;
  
  //wait the duration of the note
  delay_ms(time);
  
  //set to low
  TCTL1 = 0x00;
  
  //disable timer interrupts (stop the speaker from playing anything else)
  TIE = 0x00;
  
}

//function that plays the mii soundtrack
void play_mii() {
int i;

for(i = 0; i < 20; i++) {
  
  play_note(mii_song_notes[i], mii_song_duration[i]);
    
}

return;

}


void Init_TC6 (void) {
  //timer enable
  TSCR1 = 0x90;
  
  TSCR2 = 0x03;   // prescaler 8
  
  TIOS = 0x40;    //Output compare on channel 6
  
  
}


// variables to make the servo move back and forth
// note: This is just to demonstrate the function of the servo
long iterator_counter = 0;
int toggle = 0;




// cmd: two nibbles.
void cmd2LCD (char cmd)
{
    char hnibble, lnibble;
    PORTK = 0x00; // EN, RS=0
    // sending higher nibble
    hnibble = cmd & 0xF0; // mask first 4 bits
    PORTK = 0x02; // EN = 1
    hnibble >>=2; // shift two bits to the right to align with PORTK positions
    PORTK = hnibble|0x02;
    
    // Slight delay
    asm("nop");
    asm("nop");
    asm("nop");

    // Reset Port K after first nibble is sent
    PORTK = 0x00; // EN,RS=0
    // Sending lower nibble:
    lnibble = cmd & 0x0F;
    PORTK = 0x02;
    lnibble <<= 2;
    PORTK = lnibble|0x02;
    asm("nop");
    asm("nop");
    asm("nop");
    PORTK = 0x00;
    // Wait 50 us for order to complete (enough time for most IR instructions)
    // I realise it's 1ms here but I was just lazy to write a us delay
    delay_ms(1);
}

// Most commands here you will just have to remember
void openLCD(void)
{
    DDRK = 0xFF;   // PortK configured as outputs
    delay_ms(100); // wait for 100 ms
    cmd2LCD(0x28); // set 4-bit data, 2-line display, 5x8 font
    cmd2LCD(0x0F); // turn on display, cursor, blinking
    cmd2LCD(0x06); // move cursor right
    cmd2LCD(0x80); // set DDRAM address. DDRAM data are ent after this setting. address=0
    cmd2LCD(0x01); // clear screen, move cursor to home
    delay_ms(2);   // wait for 2 ms. The 'clear display' instruction requires this
}

void putcLCD(char cx)
{
    char hnibble, lnibble;
    PORTK = 0x01; // RS=1, EN=0
    // sending higher nibble
    hnibble = cx&0xF0;
    PORTK = 0x03; // RS=1, EN=1
    hnibble >>= 2; // shift two bits to the right to align with PORTK positions
    PORTK = hnibble|0x03;
    asm("nop");
    asm("nop");
    asm("nop");
    PORTK = 0x01; // RS=1, E=0
    // sending lower nibble
    lnibble = cx & 0x0F;
    PORTK = 0x03; // RS=1, E=1
    lnibble <<= 2;  // shift two bits to the left to align with PORTK positions
    PORTK = lnibble|0x03;
    asm("nop");
    asm("nop");
    asm("nop");
    PORTK = 0x01;// RS=1, E=0
    // Wait 50 us for order to complete (enough time for most IR instructions)
    delay_ms(1); // wait for 50  us
}

void putsLCD (char *ptr)
{
    while (*ptr)
    {
        putcLCD(*ptr);
        ptr++; 
    }
}



// the interrupt for timer 6 which is used for cycling the servo
#pragma CODE_SEG __NEAR_SEG NON_BANKED /* Interrupt section for this module. Placement will be in NON_BANKED area. */
__interrupt void TC6_ISR(void) {
  
   TC6 = TC6 + period;   
}



