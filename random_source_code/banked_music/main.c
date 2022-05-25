#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */

#include <stdio.h> 
#include <stdlib.h>

// #define for the serial port
#include "serial.h"
#include "making_music.h"

// All notes are in the 4th octave
#define A 3409
#define B 3037
#define C 5733
#define D 5108
#define E 4551
#define F 4295
#define G 3827

// Score is in the form (note, time, octave)
unsigned int score[] = {
 E, 10, 4, D, 75, 4, F, 50, 4, D, 25, 4, D, 10, 4, G, 25, 4, D, 25, 4, C, 25, 4, D, 50, 4, A, 50, 4, D, 50, 4, A, 25, 4, A, 25, 4, F, 25, 4, A, 25, 4,
 A, 25, 4, D, 25, 5, D, 10, 4, C, 25, 4, C, 10, 4, A, 25, 3, E, 25, 4, D, 50, 4
};

int time;
char time_string[10];

void main(void) {
  
  int counter = 0;
  char *string_buffer[256];
  char *print_string = "Dudes be like 'Subway Sucks', my brother in Christ, you made the sandwich!x";
  
  SerialInitialise(BAUD_9600, &SCI0);
  SerialInitialise(BAUD_9600, &SCI1);
  //set function parameters
  //&print_string[0], to also &string_buffer[0] to see result of input pushed to terminal
  SCI1_ISR(&SCI1, &SCI1, &string_buffer[0], &print_string[0]);

  time = music_runtime(score); // The song will take this many ms to play
  sprintf(&time_string[0], "%dx", time); // Converts the time integer to a string for serial output
	making_music(score);
	
	//EnableInterrupts;
  
  for(;;) {
    _FEED_COP(); /* feeds the dog */
  } /* loop forever */
  
  
  /* please make sure that you never leave main */
}


