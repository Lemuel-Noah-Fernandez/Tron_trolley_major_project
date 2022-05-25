#ifndef MUSIC_HEADER
#define MUSIC_HEADER


void timer_setup(void);
void delay(unsigned int time);
void play(unsigned int score[], int flag);
int octave_check(int octave, int letter);
int music_runtime(unsigned int score[]);
void making_music(unsigned int score[], int flag);
__interrupt void musicISR();

#endif