#include <hidef.h>      /* common defines and macros */
#include "derivative.h"

/* derivative-specific definitions */
void cmd2LCD (char cmd); // Send command to LCD
void openLCD(void); // Set up LCD
void putcLCD(char cx); // Send a single character to LCD
void putsLCD(char* ptr); // Send a string to LCD
void delay_ms(unsigned int time);  // Delay

void main(void)
{
    char* msg1 = "Testing LCD";
    char* msg2 = "Left in 3 isles";
    // Configure LCD
    openLCD();
    // Send first string
    putsLCD(msg1);
    
    // Move to new line and move cursor to the left
    // The 0xC0 is something you just will have to remember. If list of commands is found, let me know
    cmd2LCD(0xC0);
    // Send second string
    putsLCD(msg2);
    while(1);
}
// cmd: two nibbles.
void cmd2LCD (char cmd)
{
    char hnibble, lnibble;
    PORTK = 0x00; // EN, RS=0
    // sending higher nibble
    hnibble = cmd & 0xF0; // mask first 4 bits
    PORTK = 0x02; // EN = 1
    hnibble >>=2; // shift two bits to the right to align with PORTK positions
    PORTK = hnibble|0x02;
    
    // Slight delay
    asm("nop");
    asm("nop");
    asm("nop");

    // Reset Port K after first nibble is sent
    PORTK = 0x00; // EN,RS=0
    // Sending lower nibble:
    lnibble = cmd & 0x0F;
    PORTK = 0x02;
    lnibble <<= 2;
    PORTK = lnibble|0x02;
    asm("nop");
    asm("nop");
    asm("nop");
    PORTK = 0x00;
    // Wait 50 us for order to complete (enough time for most IR instructions)
    // I realise it's 1ms here but I was just lazy to write a us delay
    delay_ms(1);
}

// Most commands here you will just have to remember
void openLCD(void)
{
    DDRK = 0xFF;   // PortK configured as outputs
    delay_ms(100); // wait for 100 ms
    cmd2LCD(0x28); // set 4-bit data, 2-line display, 5x8 font
    cmd2LCD(0x0F); // turn on display, cursor, blinking
    cmd2LCD(0x06); // move cursor right
    cmd2LCD(0x80); // set DDRAM address. DDRAM data are ent after this setting. address=0
    cmd2LCD(0x01); // clear screen, move cursor to home
    delay_ms(2);   // wait for 2 ms. The 'clear display' instruction requires this
}

void putcLCD(char cx)
{
    char hnibble, lnibble;
    PORTK = 0x01; // RS=1, EN=0
    // sending higher nibble
    hnibble = cx&0xF0;
    PORTK = 0x03; // RS=1, EN=1
    hnibble >>= 2; // shift two bits to the right to align with PORTK positions
    PORTK = hnibble|0x03;
    asm("nop");
    asm("nop");
    asm("nop");
    PORTK = 0x01; // RS=1, E=0
    // sending lower nibble
    lnibble = cx & 0x0F;
    PORTK = 0x03; // RS=1, E=1
    lnibble <<= 2;  // shift two bits to the left to align with PORTK positions
    PORTK = lnibble|0x03;
    asm("nop");
    asm("nop");
    asm("nop");
    PORTK = 0x01;// RS=1, E=0
    // Wait 50 us for order to complete (enough time for most IR instructions)
    delay_ms(1); // wait for 50  us
}

void putsLCD (char *ptr)
{
    while (*ptr)
    {
        putcLCD(*ptr);
        ptr++; 
    }
}

void delay_ms(unsigned int time)
{
    int i;
    int j;
    for(i=0;i<time;i++)
      for(j=0;j<4000;j++);
}