#ifndef SERVO_H
#define SERVO_H

void PWMinitialise(void);

// sets servo in elevation and azimuth
// note: this requires verification and calibration 
void setServoPose(int azimuth, int elevation);


// interrupt used for cycling through the servo positions
__interrupt void TC6_ISR(void);

void Init_TC6 (void);

void play_note(int note_num, int duration_num);

void play_mii();

void putsLCD (char *ptr);

void putcLCD(char cx);

void openLCD(void); 

#endif