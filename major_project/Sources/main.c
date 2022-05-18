#include <hidef.h>      /* common defines and macros */
#include <assert.h>
#include "derivative.h"      /* derivative-specific definitions */

// need this for string functions
#include <stdio.h>

#include "pll.h"
#include "simple_serial.h"

#include "l3g4200d.h"

#include "servo.h"
#include "laser.h"

#include "gyro.h"


#define BUFFER 200

//global variables involved in interrupt sequence

char sentence[BUFFER];

//stores the command and allows you to enter another one   
char command[BUFFER];   

//keeps track of the length of command
int j = 0;     

//new command
int new_command = 0;



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


struct MSG_HEADER{
  int sentinel;
  char msg_type[8];
  unsigned int msg_size;
  unsigned int header_time;
  int end_sentinel;
};

struct MSG_GYRO{
  int sentinel;
  int rotation_x;
  int rotation_y;
  int rotation_z;
  unsigned int last_sample_time;
};

struct MSG_BUTTONS{
  int sentinel;
  unsigned char button_states;
  unsigned int last_press_time;
};

void SendGyroMsg(int rot_x, int rot_y, int rot_z) {
  struct MSG_HEADER gyro_header = {0xABCD, "gyro", 0, 0, 0xDCBA};
  struct MSG_GYRO gyro_message = {0x9876, 0, 0, 0, 0};
                             
  gyro_header.msg_size = sizeof(struct MSG_GYRO);
  gyro_header.header_time = TCNT;
  
  gyro_message.last_sample_time = TCNT;
  gyro_message.rotation_x = rot_x;
  gyro_message.rotation_y = rot_y;
  gyro_message.rotation_z = rot_z;
  
  SerialOutputBytes((char*)&gyro_header, sizeof(struct MSG_HEADER), &SCI1);  
  SerialOutputBytes((char*)&gyro_message, sizeof(struct MSG_GYRO), &SCI1);  
}


void delay_ms(unsigned int time){
    int i;
    int j;
    for(i=0;i<time;i++)
      for(j=0;j<4000;j++);
}


void change_servo_angle(int angle){
  int current_count = 0;
  
  while(current_count < angle*4){
    setServoPose(2 + current_count, 0);
    current_count++;
    delay_ms(1);
  }
 
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

  

  AccelRaw read_accel;
  AccelScaled scaled_accel;

  GyroRaw read_gyro;
  // MagRaw read_magnet;
  
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
  
  // initialise the sensor suite
  error_code = iicSensorInit();
  
  // write the result of the sensor initialisation to the serial
  if (error_code == NO_ERROR) {
    sprintf(buffer, "NO_ERROR\r\n");
    SerialOutputString(buffer, &SCI1);
  } else {
    sprintf(buffer, "ERROR %d\r\n");
    SerialOutputString(buffer, &SCI1);
  }

  laserInit();
  
  #else
  
  #endif

  Init_TC6();
  
	EnableInterrupts;
  //COPCTL = 7;
  _DISABLE_COP();
  setServoPose(0, 0);
    
  for(;;) {
  
    current_time = TCNT;
  
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
    
    //error_code = getRawDataMagnet(&read_magnet);
    
    GetLatestLaserSample(&singleSample);
        
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
    
    
    if(count%100 == 0){
      
    sprintf(buffer,"w is %d, angle is %f\n", omega, angle);
    SerialOutputString(buffer, &SCI1);
    }
     
    //format the string of the sensor data to go the the serial    
    //sprintf(buffer,"%f, %d\r\n",float_time_diff, read_gyro.x);
   
    // output the data to serial
    //SerialOutputString(buffer, &SCI1);
    
    //SendGyroMsg(read_gyro.x, read_gyro.y, read_gyro.z);
    
    
    
    /*
    if(count%100 == 0){
    sprintf(buffer, "time: %f, accel: %.2f velocity: %f, displacement: %f\n",float_time_diff, 9.8*scaled_accel.y, velocity, distance);
    SerialOutputString(buffer, &SCI1);
    }
    
     */
    
    
    //change_servo_angle(0);
    
    /*
    if(finish_flag == 0){
      change_servo_angle(0);
      delay_ms(100);
      finish_flag = 1;
    } else if(finish_flag == 1){
      change_servo_angle(30);
      delay_ms(100);
      finish_flag = 2; 
    }
    */
    
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

