#include <hidef.h>       /* common defines and macros */
#include <math.h>
#include <stdlib.h>
#include "derivative.h"      /* derivative-specific definitions */
#include "making_music.h"

int note;
int i = 0;
int j;
int k;
//int flag = 0; // Flag at 1 when playing a song, 0 when ready to receive new tune


void making_music(unsigned int score[], int flag) {
  
  timer_setup();
  play(score, flag);
  for(;;);
  
}

// Initialises timer
void timer_setup(void) {
  
  TSCR1 = 0x90; // Enable TCNT to fast timer flag clear
  TSCR2 = 0x03; // Set TCNT clock prescaler to 8
  TIOS = 0x20;  // Output compare to chanel 5
  TCTL1 = 0x04; // Toggles on successful output compare

}


// Play music
void play(unsigned int score[], int flag) {
  int x = 0;
  while(score[x]) {
    flag = 1; // Flag = 1 while music is playing
    if(x == 0) {
      int letter = score[0];
      int time = score[1];
      int octave = score[2];
      letter = octave_check(octave, letter);
      note = letter; 
      //delay(time);
      //note = 20;
      //delay(10);
      TIE = 0x20;   // Enable interrupts for timer 5
     
      // First output compare
      TC5 = TCNT + note;
      delay(10);
    	EnableInterrupts;            
    }else if(x % 3 == 0) {
      int letter = score[x];
      int time = score[x+1];
      int octave = score[x+2];
      if(time > 1000) {
        break;
      }
      letter = octave_check(octave, letter);
      note = letter;
      delay(time); 
      note = 20;
      delay(10);    
    }
    x++;
      
  }
  flag = 0; // Set flag back to 0
  /*
  note = 20;
  delay(10);
  */
}


  // Decoding score
  /*
  int x;
  for(x=0; x<23; x++;) {
    long int num = score[x];
    long int letter = floor(num/10000);
    long int time = num % 1000;    
    
    int octave = 0;
    for(int y=0; y<8; y++){
        if(y == 3){
        octave = num % 10;
        }else{
            num % 10;
        }
        num /= 10;
    }
    
    if(x == 0) {
      note = letter; 
      delay(dur[time]);
      i++;
      TIE = 0x20;   // Enable interrupts for timer 5
     
      // First output compare
      TC5 = TCNT + note;
    	EnableInterrupts;
    }else {
      note = letter;
      delay(dur[time]);
      i++;      
    }
    note = 20;   
  }
  */

  

// Octave check
int octave_check(int octave, int letter){
  switch (octave)
  {
      case 5:
      letter /= 2;
      break;
      case 6:
      letter /= 4;
      break;
      case 7:
      letter /= 8;
      break;
      case 8:
      letter /= 16;
      break;
      case 3:
      letter *= 2;
      break;
      case 2:
      letter *= 4;
      break;
      case 1:
      letter *= 8;
      break;
      case 0:
      letter *= 16;
      break;
  }
  return letter;
}

// Delay function
void delay(unsigned int time) {
      
    // Delay of time in ms x 10
    time *= 10;
    //time *= 5;
    for(j=0;j<time;j++)
      for(k=0;k<4000;k++);
}


// Music run time function
int music_runtime(unsigned int score[]) {
  int x = 0;
  int total_time = 0;
  int time_for_note;
  while(score[x]) {
    
    if(x == 0) {
      time_for_note = score[1];
      total_time += time_for_note;
    }else if(x % 3 == 0) {
      time_for_note = score[x+1];
      total_time += time_for_note;
    }
    x++;
  }
  total_time *= 10;
  return total_time;
}

// Interrupt vector
#pragma CODE_SEG __NEAR_SEG NON_BANKED
__interrupt void musicISR()
{
	TC5 = TC5 + note;
}