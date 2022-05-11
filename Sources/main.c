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
  volatile float distance = 0;
  volatile float velocity = 0;
  int count = 0;
  

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
  setServoPose(100, 100);

  #endif
  
  // initialise the simple serial
  SerialInitialise(BAUD_9600, &SCI1);
  
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
    
    
     
    // format the string of the sensor data to go the the serial    
    //sprintf(buffer,"%lu, %d, %d, %d, %.2f, %.2f, %.2f\r\n",singleSample, read_gyro.x, read_gyro.y, read_gyro.z, scaled_accel.x, scaled_accel.y, scaled_accel.z);
   
    // output the data to serial
    //SerialOutputString(buffer, &SCI1);
    
    //sprintf(buffer, "%f - y acceleration\n", scaled_accel.y);
    //SerialOutputString(buffer, &SCI1);
    
    
    if(count%100 == 0){
    sprintf(buffer, "time: %f, accel: %.2f velocity: %f, displacement: %f\n",float_time_diff, 9.8*scaled_accel.y, velocity, distance);
    SerialOutputString(buffer, &SCI1);
    }

                            
    //perform integration
    velocity = (float)velocity + (float)9.8*(float)float_time_diff*(float)scaled_accel.y;
    distance = distance + float_time_diff*velocity;
    

    
    previous_time = current_time;
    count++;
    
    
    //_FEED_COP(); /* feeds the dog */
  } /* loop forever */
  
  /* please make sure that you never leave main */
}
