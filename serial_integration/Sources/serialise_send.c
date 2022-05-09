#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */

#include <stdio.h>
#include <stdlib.h>

#include "simple_serial.h"
#include "serialise_send.h"

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

void SendButtonsMsg() {
  struct MSG_HEADER button_header = {0xABCD, "buttons", 0, 0, 0xDCBA};
  struct MSG_BUTTONS button_message = {0x4321, 0x0A, 0};
                             
  button_header.msg_size = sizeof(struct MSG_BUTTONS);
  button_header.header_time = TCNT;
  
  button_message.last_press_time = TCNT;
  
  SerialOutputBytes((char*)&button_header, sizeof(struct MSG_HEADER), &SCI1);  
  SerialOutputBytes((char*)&button_message, sizeof(struct MSG_BUTTONS), &SCI1);   
}

void SendTextMsg(char* text_message) {
  struct MSG_HEADER text_header = {0xABCD, "text", 0, 0, 0xDCBA};
  text_header.msg_size = strlen(text_message);
  text_header.header_time = TCNT;
  
  SerialOutputBytes((char*)&text_header, sizeof(struct MSG_HEADER), &SCI1);  
  SerialOutputBytes(text_message, text_header.msg_size, &SCI1);
}

void main_serialise(void) {
  char text_buffer[32];
  int rot_x = 0;
  int rot_y = 10;
  int rot_z = 100;

  _DISABLE_COP();

  
  // initialise the serial
  SerialInitialise(BAUD_115200, &SCI1);
  
  // initialise the timer
  TSCR1_TEN = 1;  
  TSCR2_PR = 0b111;
  
  sprintf(text_buffer, "first 12345");
  SendTextMsg(text_buffer);
  SendTextMsg(text_buffer);
  SendTextMsg(text_buffer);
  
  sprintf(text_buffer, "second 123456");
  SendTextMsg(text_buffer);
  SendTextMsg(text_buffer);
  SendTextMsg(text_buffer);
  
	EnableInterrupts;

  SendButtonsMsg();
  SendButtonsMsg();
  SendButtonsMsg();
  
  _DISABLE_COP();
   
}