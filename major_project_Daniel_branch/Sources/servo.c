
#include "derivative.h"
#include <math.h> 
#include "servo.h"
#include "simple_serial.h"


#define ZERO_ELEVATION_DUTY 4600
#define ZERO_AZIMUTH_DUTY 2000


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



int mii_song_notes[] = {16, 19, 23, 19, 16, 12, 0, 12, 0, 12, 0, 16, 19, 23, 0, 19, 16, 26, 25, 24};  //20
int mii_song_duration[] = {5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 3, 5, 5, 5, 6, 5, 5, 4, 5, 5};


void play_note(int note_num, int duration_num) {
  
  int time = duration[duration_num]*125;
  period = note[note_num];
  
  //toggle on successful output compare
	TCTL1 = 0x04;
  
  //enable timer interrupts
  TIE = 0x20;
  
  //set first output compare to happen 
	TC5 = TCNT + period;
  
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
  TSCR1_TEN = 1;
  
  TSCR2 = 0x00;   // prescaler 1, before 32 = 0x04
  TIOS_IOS6 = 1;   // set channel 6 to output compare
    
  TCTL1_OL6 = 1;    // Output mode for ch6
  //TIE_C6I = 1;   // enable interrupt for channel 6

  TFLG1 |= TFLG1_C6F_MASK;
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
  
  
  int angle_to_reach = 45;
  int angle_input = 4*angle_to_reach;
  TC6 = TCNT + 64000;   // interrupt delay depends on the prescaler
  TFLG1 |= TFLG1_C6F_MASK;
  
  
  if (toggle == 0)
    iterator_counter++;
  else
    iterator_counter--;
  
  if (iterator_counter > angle_input) {
    toggle = 1;
  } else if (iterator_counter < 0) {
    toggle = 0;
  }
  
  
  
  setServoPose(2 + iterator_counter, 0);    
}

