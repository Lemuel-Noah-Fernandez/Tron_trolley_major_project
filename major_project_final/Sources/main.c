#include <hidef.h>      /* common defines and macros */
#include <assert.h>
#include "derivative.h"      /* derivative-specific definitions */

// need this for string functions
#include <stdio.h>
#include <stdlib.h>

#include "pll.h"
#include "simple_serial.h"

#include "l3g4200d.h"

#include "servo.h"
#include "serialise_send.h"
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

char text_buffer[32];


#define BUFFER 200

//global variables involved in interrupt sequence

volatile char sentence[BUFFER];

//stores the command and allows you to enter another one   
char command[BUFFER];   

//keeps track of the length of command
int j = 0;     

//new command
volatile int new_command = 0;



void sevensegmodule(int number){
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

  
  
  return;   
}





void main(void) {
  
 
  int count = 0;
  int desired_angle = 0;
  int previous_angle = 0;
  
  
  int isle_number = 0;
  int data[10];
  int data_count = 0;
  
  char* LCD_buffer[100];
  int sevensegnum;


  char buffer[200];  
  
  
  
  unsigned long singleSample;
  
  
  //assert(error_code != NO_ERROR);

  #ifndef SIMULATION_TESTING

  // make sure the board is set to 24MHz
  //  this is needed only when not using the debugger
  PLL_Init();

  // initialise PWM
  PWMinitialise();

  #endif
  
  // initialise the simple serial
  SerialInitialise(BAUD_115200, &SCI1);
  
	EnableInterrupts;
  //COPCTL = 7;
  _DISABLE_COP();
  setServoPose(165, 0);
  
 
	EnableInterrupts;
    
    
    
  sevensegmodule(0);
  
  for(;;) {
  
    
    if(new_command == 1){
    
   
    //LCD processing
    if(command[0] == 'L'){
    
    strcpy(LCD_buffer, command + 2);
    openLCD();
    putsLCD(LCD_buffer); 
    
    }
    
    //Angle Processing
    if(command[0] == 'A'){
    
    desired_angle = atoi(command + 2);
    previous_angle = desired_angle;
    setServoPose(2 + desired_angle, 0);
    }
    
   
    //Tilt unit processing 
    if(command[0] == 'T'){
      
      desired_angle = atoi(command + 2);
      setServoPose(previous_angle, 100*desired_angle);
    }
    
    //music unit
    if(command[0] == 'M'){
      making_music(score, 0);
    }
    
    //Seven Seg Module
    if(command[0] == 'S'){
     
     sevensegnum = atoi(command + 2);
     sevensegmodule(sevensegnum);
    }
    
    //request aisle
    if(command[0] == 'R'){
      
      isle_number++;
      if(isle_number == 6){
        isle_number == 0;
      }
      
      //sevensegmodule(isle_number);
      sprintf(buffer, "aisle: %d", isle_number);
      SendTextMsg(buffer);
    }
    
   
    
    new_command = 0;
    
    }
   
    count++;
    
    
    //_FEED_COP(); /* feeds the dog */
  } /* loop forever */
  
  /* please make sure that you never leave main */
}


#pragma CODE_SEG __NEAR_SEG NON_BANKED /* Interrupt section for this module. Placement will be in NON_BANKED area. */
__interrupt void Serial_ReadISR(void){


  int i = 0;
  int k = 0;
  
  char* error_sentence = "You've exceeded the buffer limit. Try again!\n";
  int length = strlen(error_sentence);
  
  // Check if data is received. The RDRF flag
  if (SCI1SR1 & 0x20) 
  {
 
    
    // End of sentence? Look for a carriage return
    if (SCI1DRL == 0x0D) 
    {
    
      //add in null terminator to string    
      sentence[j] = '\0';
      
      
      
      //copies sentence to command 
      strcpy(command, sentence);
      
      //sets command to 1 to make sure program knows there is a new command that hasn't been read
      new_command = 1;
     
      //output the newline to terminal
      SerialOutputChar(0x0D, &SCI1);
      
      // Reset buffer
      j = 0;
      
      
    } 
    
    // Store each character of sentence in buffer
    else
    {
      sentence[j] = SCI1DRL;
      
      
      //output the character to the serial so it can be seen
      SerialOutputChar(sentence[j], &SCI1);
      
      
      //incrememnt length of word
      j = j + 1;
      
      //buffer overflow condition
      if(j == BUFFER) {
      
         for (i = 0; i < length; i++) 
        {
          // Wait for data to be ready
          while(!(SCI1SR1 & 0x80));
          
          // Write to serial
         SCI1DRL = error_sentence[i];
          
        }
      
       j = 0; //Reset buffer
        
      }
    }
  }


}
