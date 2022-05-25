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
#include "laser.h"

#include "gyro.h"
#include "serialise_send.h"

char text_buffer[32];
int rot_x = 0;
int rot_y = 10;
int rot_z = 100;


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

int test_changed(int prevx, int prevy, int prevz, MagRaw readmagnet){

  if(prevx - readmagnet.x > 10000){
    return 1;
  }
  if(prevx - readmagnet.x < -10000){
    return 1;
  }
  if(prevy - readmagnet.y > 10000){
    return 1;
  }
  if(prevy - readmagnet.y < -10000){
    return 1;
  }
  if(prevz - readmagnet.z > 10000){
    return 1;
  }
  if(prevz - readmagnet.z < -10000){
    return 1;
  }
  
  return 0;
  
}


void printErrorCode(IIC_ERRORS error_code) {
  char buffer[128];  
  switch (error_code) {
    case NO_ERROR: 
      sprintf(buffer, "IIC: No error\r\n");
      break;
    
    case NO_RESPONSE: 
      sprintf(buffer, "IIC: No response\r\n");
      break;
    
    case NAK_RESPONSE:
      sprintf(buffer, "IIC: No acknowledge\r\n");
      break;
    
    case IIB_CLEAR_TIMEOUT:
      sprintf(buffer, "IIC: Timeout waiting for reply\r\n");
      break;
    
    case IIB_SET_TIMEOUT: 
      sprintf(buffer, "IIC: Timeout not set\r\n");
      break;
    
    case RECEIVE_TIMEOUT:
      sprintf(buffer, "IIC: Received timeout\r\n");
      break;
    
    case IIC_DATA_SIZE_TOO_SMALL:
      sprintf(buffer, "IIC: Data size incorrect\r\n");
      break;

    default:
      sprintf(buffer, "IIC: Unknown error\r\n");
      break;
  }
    
  SerialOutputString(buffer, &SCI1);
}


void main(void) {
  
  volatile unsigned int previous_time;
  volatile unsigned int current_time;
  volatile unsigned int time_diff;
  volatile float float_time_diff;
  volatile float angle = 0;
  volatile float scaled_angle = 0;
  volatile int omega;
  int count = 0;
  int finish_flag = 0;
  int desired_angle = 0;
  int previous_angle = 0;
  
  int previous_xmag = 0;
  int previous_ymag = 0;
  int previous_zmag = 0;
  
  int isle_number = 0;
  int data[10];
  int data_count = 0;
  
  char* LCD_buffer[100];
  int sevensegnum;

  

  AccelRaw read_accel;
  AccelScaled scaled_accel;

  GyroRaw read_gyro;
  MagRaw read_magnet;
  
  IIC_ERRORS error_code = NO_ERROR;
  
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
  
  #ifndef SIMULATION_TESTING
  
  
  laserInit();
  
  #else
  
  #endif

  //Init_TC6();
  
	EnableInterrupts;
  //COPCTL = 7;
  _DISABLE_COP();
  setServoPose(165, 0);
  
 
	EnableInterrupts;
    
    
    
  sevensegmodule(0);
  
  sprintf(buffer, "aisle: 1");
  
  
  /*
  while(1){
   SendTextMsg(buffer);
  }
 
  */
  
  //change_servo_angle(50, 1); 
  
  for(;;) {
    
    
    
  
    //SendGyroMsg(rot_x, rot_y, rot_z);
  
    //current_time = TCNT;
    
    
    /*
  
    #ifndef SIMULATION_TESTING
  
    // read the raw values
    error_code = getRawDataGyro(&read_gyro);   
    if (error_code != NO_ERROR) {
      printErrorCode(error_code);   
       
      error_code = iicSensorInit();
      printErrorCode(error_code);   
    }
    
    error_code = getRawDataAccel(&read_accel);
    if (error_code != NO_ERROR) {
      printErrorCode(error_code);   
    
      error_code = iicSensorInit();
      printErrorCode(error_code); 
    }
    
    error_code = getRawDataMagnet(&read_magnet);
    
    if(previous_xmag == 0){
      previous_xmag = read_magnet.x;
      previous_ymag = read_magnet.y;
      previous_zmag = read_magnet.z;

    }
    
        
    #else
    
    
    // inject some values for simulation
    read_gyro.x = 123; read_gyro.y = 313; read_gyro.z = 1002;
    read_accel.x = 124; read_accel.y = 312; read_accel.z = 2002;
    read_magnet.x = 125; read_magnet.y = 311; read_magnet.z = 3002;
    
    #endif

    // convert the acceleration to a scaled value
    convertUnits(&read_accel, &scaled_accel);    
    
    
    time_diff = current_time - previous_time;
    float_time_diff = (float)time_diff/(24000000.0);

    
    if(time_diff > 0){
      //print out the time
      //sprintf(buffer, "time: %f current_time: %lu previous_time: %lu\n", float_time_diff, current_time, previous_time);
      //SerialOutputString(buffer, &SCI1); 
    }else{
    
      //if(current_time < 0){
      //  current_time = 65535 - current_time; 
      time_diff = 65535 - previous_time + current_time;
      //sprintf(buffer, "time: %f current_time: %lu previous_time: %lu\n", float_time_diff, current_time, previous_time);
      //SerialOutputString(buffer, &SCI1); 
      
    }
    
    
    
    
    if(read_gyro.x < -10000) {
      omega = 0;
    } else{
      omega = read_gyro.x - 23;
    }
    
    if(read_gyro.x > 10000) {
      omega = 0;
    }else{
      omega = read_gyro.x - 23;
    }
    
    //scale the angular velocity
    omega = omega/14;
    angle = angle + float_time_diff* (float)omega;
    scaled_angle = angle/13.88888;
    
   
    /*
    if(test_changed(previous_xmag, previous_ymag, previous_zmag, read_magnet) == 1){
      isle_number++;
      sprintf(buffer, "aisle: %d\n", isle_number);
      SerialOutputString(buffer, &SCI1);
      //SendTextMsg(buffer);
     
    }
    
    
    
    if(count%100 == 0){
    
    //print angle values  
    sprintf(buffer,"w is %d, angle is %f\n", omega, angle);
    
    sprintf(buffer, "aisle: %d", isle_number);
    //SendTextMsg(buffer);
    //isle_number++;
    
    
    //print magnet data
    //sprintf(buffer, "reading, diff z = %d, %d, y = %d, %d, x = %d, %d\n", read_magnet.z, previous_zmag - read_magnet.z, read_magnet.y, previous_ymag - read_magnet.y, read_magnet.x, previous_xmag - read_magnet.x); 
    
   
    
    //SerialOutputString(buffer, &SCI1);
    
    }
     

  

    if(count%5 == 0){
      previous_xmag = read_magnet.x;
      previous_ymag = read_magnet.y;
      previous_zmag = read_magnet.z;
    }
    
    */
    
    
    
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
    
    if(command[0] == 'M'){
      Init_TC6();
      play_mii();
    }
    
    //Seven Seg Module
    if(command[0] == 'S'){
     
     sevensegnum = atoi(command + 2);
     //sevensegmodule(sevensegnum);
    }
    
    //request aisle
    if(command[0] == 'R'){
      
      isle_number++;
      if(isle_number == 6){
        isle_number == 0;
      }
      
      sevensegmodule(isle_number);
      sprintf(buffer, "aisle: %d", isle_number);
      SendTextMsg(buffer);
    }
    
   
    
    new_command = 0;
    
    }
   
    previous_time = current_time;
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
