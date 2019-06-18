#include <wiringPi.h>
#include <stdint.h>
#include <stdio.h>
/*

 +-----+-----+---------+------+---+---Pi 2---+---+------+---------+-----+-----+
 | BCM | wPi |   Name  | Mode | V | Physical | V | Mode | Name    | wPi | BCM |
 +-----+-----+---------+------+---+----++----+---+------+---------+-----+-----+
 |     |     |    3.3v |      |   |  1 || 2  |   |      | 5v      |     |     |
 |   2 |   8 |   SDA.1 | ALT0 | 1 |  3 || 4  |   |      | 5v      |     |     |
 |   3 |   9 |   SCL.1 | ALT0 | 1 |  5 || 6  |   |      | 0v      |     |     |
 |   4 |   7 | GPIO. 7 |   IN | 1 |  7 || 8  | 1 | ALT0 | TxD     | 15  | 14  |
 |     |     |      0v |      |   |  9 || 10 | 1 | ALT0 | RxD     | 16  | 15  |
 |  17 |   0 | GPIO. 0 |   IN | 0 | 11 || 12 | 0 | OUT  | GPIO. 1 | 1   | 18  |
 |  27 |   2 | GPIO. 2 |   IN | 0 | 13 || 14 |   |      | 0v      |     |     |
 |  22 |   3 | GPIO. 3 |   IN | 0 | 15 || 16 | 0 | IN   | GPIO. 4 | 4   | 23  |
 |     |     |    3.3v |      |   | 17 || 18 | 0 | IN   | GPIO. 5 | 5   | 24  |
 |  10 |  12 |    MOSI | ALT0 | 0 | 19 || 20 |   |      | 0v      |     |     |
 |   9 |  13 |    MISO | ALT0 | 0 | 21 || 22 | 0 | IN   | GPIO. 6 | 6   | 25  |
 |  11 |  14 |    SCLK | ALT0 | 0 | 23 || 24 | 1 | OUT  | CE0     | 10  | 8   |
 |     |     |      0v |      |   | 25 || 26 | 1 | OUT  | CE1     | 11  | 7   |
 |   0 |  30 |   SDA.0 |   IN | 1 | 27 || 28 | 1 | IN   | SCL.0   | 31  | 1   |
 |   5 |  21 | GPIO.21 |   IN | 1 | 29 || 30 |   |      | 0v      |     |     |
 |   6 |  22 | GPIO.22 |   IN | 1 | 31 || 32 | 0 | IN   | GPIO.26 | 26  | 12  |
 |  13 |  23 | GPIO.23 |   IN | 0 | 33 || 34 |   |      | 0v      |     |     |
 |  19 |  24 | GPIO.24 |   IN | 0 | 35 || 36 | 0 | IN   | GPIO.27 | 27  | 16  |
 |  26 |  25 | GPIO.25 |   IN | 0 | 37 || 38 | 0 | IN   | GPIO.28 | 28  | 20  |
 |     |     |      0v |      |   | 39 || 40 | 0 | IN   | GPIO.29 | 29  | 21  |
 +-----+-----+---------+------+---+----++----+---+------+---------+-----+-----+
 | BCM | wPi |   Name  | Mode | V | Physical | V | Mode | Name    | wPi | BCM |
 +-----+-----+---------+------+---+---Pi 2---+---+------+---------+-----+-----+


D0 -> D7 ==> GPIO.0 - GPIO.7
RS -> GPIO.27
RW -> GPIO.28
EN -> GPIO.29

*/

/* index */
#define D0      0
#define D1      1
#define D2      2
#define D3      3
#define D4      4
#define D5      5
#define D6      6
#define D7      7
#define RS      8   // register select
#define RW      9   // RW
#define EN      10  // Enable

#define MAX_PIN     11
#define X           0 /* don't care, just set as pin 0 for now */
/* add one more abstraction layer for debug */
                           /*  D0  D1  D2  D3  D4  D5 D6 D7  RS  RW  EN -> wPi */
uint8_t mapping_0[MAX_PIN] = {  0,  1,  2,  3,  4,  5, 6, 7, 27, 28, 29};
uint8_t mapping_1[MAX_PIN] = { 21, 22, 23, 24, 25, 26, X, X, 12, 13, 14}; /* take SPI! */
uint8_t *mapping = NULL;
#undef X

/* No output please! */
void LCD_Pin_Setup()
{
    mapping = mapping_0;
    pinMode(mapping[D0], OUTPUT);
    pinMode(mapping[D1], OUTPUT);
    pinMode(mapping[D2], OUTPUT);
    pinMode(mapping[D3], OUTPUT);
    pinMode(mapping[D4], OUTPUT);
    pinMode(mapping[D5], OUTPUT);
    pinMode(mapping[D6], OUTPUT);
    pinMode(mapping[D7], OUTPUT);
    pinMode(mapping[RS], OUTPUT);
    pinMode(mapping[RW], OUTPUT);
    pinMode(mapping[EN], OUTPUT);
    for (uint8_t i = 0; i < 8; i++) {
        digitalWrite(mapping[i], 1);
    }
    digitalWrite(mapping[RS], 0);
    digitalWrite(mapping[RW], 1);
    digitalWrite(mapping[EN], 0);
}

void OutDataBus(uint8_t dat)
{
    uint8_t tem = dat;
    for (uint8_t i = 0; i < 8; i++) {
        digitalWrite(mapping[i], tem & 0x01);
        tem = tem >> 1;
    }
    
}

void Lcd_CmdWrite( char cmd)
{
    OutDataBus(cmd);
    digitalWrite(mapping[RS], 0);    /* Command register */
    digitalWrite(mapping[RW], 0);    /* write */
    /* falling edge */
    digitalWrite(mapping[EN], 1);
    delay(100);
    digitalWrite(mapping[EN], 0);
    delay(100);
    printf("Done command\n");
}

void Lcd_DataWrite( char dat)
{
    OutDataBus(dat);
    digitalWrite(mapping[RS], 1);    /* Data register */
    digitalWrite(mapping[RW], 0);    /* write */
    /* falling edge */
    digitalWrite(mapping[EN], 1);
    delay(100);
    digitalWrite(mapping[EN], 0);
    delay(100);
    printf("done Data\n");
}

int main (void)
{
    char i;
    char a[]={"Good morning!"};

    wiringPiSetup () ;
    LCD_Pin_Setup();

#if 0
    Lcd_CmdWrite(0x38);
    Lcd_CmdWrite(0x06); // entry mode set: increment automatically, no display shift
    Lcd_CmdWrite(0x0E);
    Lcd_CmdWrite(0x01);
    Lcd_DataWrite('H');
#endif

#if 1
    Lcd_CmdWrite(0x38);  //function set: 8-bit interface, 2 display lines, 5x7 font
    Lcd_CmdWrite(0x06); // entry mode set: increment automatically, no display shift
    Lcd_CmdWrite(0x0E); // Display OFF, Cursor ON
    Lcd_CmdWrite(0x01); // Clear Display
    Lcd_CmdWrite(0x80); // Move the cursor to beginning of first line
    Lcd_DataWrite('H');
    Lcd_DataWrite('e');
    Lcd_DataWrite('l');
    Lcd_DataWrite('l');
    Lcd_DataWrite('o');
    Lcd_DataWrite(' ');
    Lcd_DataWrite('w');
    Lcd_DataWrite('o');
    Lcd_DataWrite('r');
    Lcd_DataWrite('l');
    Lcd_DataWrite('d');
#endif

#if 1
    Lcd_CmdWrite(0xC0); //Go to Next line and display Good Morning

    for(i=0; a[i]!=0 ;i++) {
        Lcd_DataWrite(a[i]);
    }
#endif
    //while (1);
}