void play_note(int note_num, int duration_num);
void play_mii();

#define numnotes 31
#define numdurations 7
//define each of the periods used for the notes

#define rest 20
#define Eb3 9641
#define E3  9100
#define F3  8589
#define Gb3 8107
#define G3  7652
#define Ab3 7223
#define A3  6817
#define Bb3 6434
#define B3  6073
#define C4  5733
#define Db4 5411
#define D4  5107
#define Eb4 4820
#define E4  4550
#define F4  4294
#define Gb4 4053
#define G4  3826
#define Ab4 3611
#define A4  3408
#define Bb4 3611
#define B4  3036
#define C5  2866
#define Db5 2705
#define D5  2553
#define Eb5 2410
#define E5  2275
#define F5  2147
#define Gb5 2026
#define G5  1913
#define Ab5 1805
#define A5  1704


#define longnote  128      
#define dbreve    64
#define breve     32
#define semibreve 16
#define minim     8
#define crotchet  4
#define quaver    2
#define semiquav  1


                                    //index's 
                                    
unsigned int note[] = {rest,        //0
    Eb3,                            //1
    E3,                             //2
    F3,                             //3
    Gb3,                            //4
    G3,                             //5
    Ab3,                            //6
    A3,                             //7
    Bb3,                            //8
    B3,                             //9
    C4,                             //10
    Db4,                            //11
    D4,                             //12
    Eb4,                            //13
    E4,                             //14
    F4,                             //15
    Gb4,                            //16
    G4,                             //17
    Ab4,                            //18
    A4,                             //19
    Bb4,                            //20
    B4,                             //21
    C5,                             //22
    Db5,                            //23
    D5,                             //24
    Eb5,                            //25
    E5,                             //26
    F5,                             //27
    Gb5,                            //28
    G5,                             //29
    Ab5                             //30
    };

unsigned int duration[] =
{
    dbreve,
    breve,
    semibreve,
    minim,
    crotchet, 
    quaver,
    semiquav
};

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
