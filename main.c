/*

CPU:
MSP430G2452
MSP430G2553
MSP430*****

LCD:        SPI 12 Digit (can be changed in the #define section)
LCD type:   "12Digit 7Seg SPI LCD LED" from eBay, $4.99 as of 9/2016
LCD pinout: CS WR DA GND VCC (from left to right, LCD facing up)

PORTS:
P1.0    DA
P1.1    WR
P1.2    CS

*/

#include <msp430.h>
#include <stdio.h>

#define uint  unsigned int
#define uchar  unsigned char
#define nop __no_operation()

#define LCD_CS BIT2
#define LCD_WR BIT1        
#define LCD_DA BIT0

#define LCD_DIGITS 12

#define  BIAS     0x52
#define  SYSDIS   0X00
#define  SYSEN    0X02
#define  LCDOFF   0X04
#define  LCDON    0X06
#define  XTAL     0x28
#define  RC256    0X30
#define  TONEON   0X12
#define  TONEOFF  0X10
#define  WDTDIS1  0X0A

//7-seg symbols definition (define your own here)
const char n[] = {
  0b10111110, //0
  0b00000110, //1
  0b01111100, //2
  0b01011110, //3
  0b11000110, //4
  0b11011010, //5
  0b11110010, //6
  0b00001110, //7
  0b11111110, //8
  0b11001110, //9
  0b00000001, //.
  0b01000000, //-
  0b11001100, //o
  0b01110010, //c
};

//mcu init
void init(void){
    WDTCTL = WDTPW + WDTHOLD;
    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;
    BCSCTL2 &= ~(DIVS_3);
    P1DIR=0xFF;
    P1OUT=0xFF;
}

//send data to lcd
void lcd_data(uchar b,uchar c){ 
    uchar i;
    for (i=0;i<c;i++){   
        P1OUT &= ~LCD_WR; 
        nop; nop; nop; nop;
        if((b & 0x80)==0x80){P1OUT |=LCD_DA;}
        else{P1OUT &=~LCD_DA;}
        P1OUT |=LCD_WR; 
        nop; nop; nop; nop;
        b<<=1;  
        }
    }

//send command to lcd
void lcd_cmd(uchar c){ 
  P1OUT&=~LCD_CS;
  nop; nop; nop; nop;
  lcd_data(0x80,4);
  lcd_data(c,8);
  P1OUT |= LCD_CS; 
} 

//clear lcd
void lcd_clear(void){
    uchar i;
    P1OUT &=~LCD_CS;
    lcd_data(0xa0,3);
    lcd_data(0x00<<2,6);
    for (i=0;i<LCD_DIGITS;i++){ lcd_data(0x00,8); } 
    P1OUT |=LCD_CS;   
}

//lcd turn on sequence
void lcd_setup(void){ 
    P1OUT |=LCD_CS; 
    P1OUT |=LCD_WR; 
    P1OUT |=LCD_DA; 
    __delay_cycles(2000);
    lcd_cmd(BIAS); 
    lcd_cmd(RC256);
    lcd_cmd(SYSDIS); 
    lcd_cmd(WDTDIS1); 
    lcd_cmd(SYSEN); 
    lcd_cmd(LCDON); 
    __delay_cycles(1000);
} 

//send byte to lcd
void lcd_byte(uchar a,uchar b){ 
    P1OUT &=~LCD_CS; 
    lcd_data(0xa0,3);
    lcd_data(a<<2,6);
    lcd_data(b,8);
    P1OUT |=LCD_CS;
}

//print number from string
void prints(char *c){
    uchar i=0;
    while(*c != '\0'){
        if (*c=='-') *c=59;
        lcd_byte(i<<1,n[*c-48]);
        i++; c++;
    }
}

//generic integer-to-array
void itoa(long int v, char* r){
    char* ptr = r, *ptr1 = r, c;
    int t;

    do{
        t = v;
        v /= 10;
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (t - v * 10)];
        } while(v);

    if (t < 0) *ptr++ = '-';
    *ptr-- = '\0';
    while(ptr1 < ptr){
        c = *ptr;
        *ptr--= *ptr1;
        *ptr1++ = c;
        }
    }


//print number from int
printn(long int v){
    char a[20];
    itoa(v,a);
    prints(a);
}

//main begin
void main(void){

    init();
    lcd_setup();
    lcd_clear();
    
    __delay_cycles(100000); 
    prints("123");                  //number as string
    __delay_cycles(1000000);
    
    __delay_cycles(100000);
    printn(4567890);                //number as int
    __delay_cycles(1000000);
    
    __delay_cycles(100000);
    printn(-1234567890);            //negative number as int
    __delay_cycles(1000000);

    // main end: cpu hangs here
}
