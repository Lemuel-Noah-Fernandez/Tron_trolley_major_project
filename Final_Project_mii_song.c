void play_note(int note_num, int duration_num);
void play_mii();

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
