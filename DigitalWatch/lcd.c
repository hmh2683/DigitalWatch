#include <avr/io.h>
#include <util/delay.h>
#include "lcd.h"
 
#define LCD_PORT PORTC       
#define LCD_DDR  DDRC
   
static void write_command(char command);
static void write_data(char ch);
static void checkbusy(void);    

void LcdInit(void){             
   MCD_DDR = 0xFF;

   _delay_ms(15);
   write_command(0x38);
   _delay_ms(5);
   write_command(0x38);
   _delay_ms(1);
   write_command(0x38);
   LcdCommand(FUNSET);
   LcdCommand(DISP_OFF);
   LcdCommand(ALLCLR);
   LcdCommand(ENTMOD);
   LcdCommand(DISP_ON);
}

void LcdCommand(char command){           
   checkbusy();
   write_command(command);
   if (command == ALLCLR || command == HOME)
       _delay_ms(2);    // 명령실행시간 1.52ms
}
void LcdMove(char line, char pos){            
   pos = (line << 6) + pos;   // DDRAM 주소 계산
   pos |= 0x80;               // DDRAM 주소 설정 명령 (LCD 명령표 참조)
   LcdCommand(pos);
} 

void LcdPutchar(char ch){        
   checkbusy();
   write_data(ch);
}
 
void LcdPuts (char* str){                
   while(*str){
      LcdPutchar(*str);
      str++;
   }
}

static void write_command(char command){    
   char temp;
   temp = (command & 0xF0) | 0x04;     // output upper nibble (RS=0, RW=0, E=1)
   LCD_PORT = temp;
   LCD_PORT = temp & ~0x04;      // E=0
   temp = (command << 4) | 0x04;       // output lower nibble (RS=0, RW=0, E=1)
   LCD_PORT = temp;
   LCD_PORT = temp & ~0x04;      // E=0
}
static void write_data(char ch){   
   unsigned char temp;
   temp = (ch & 0xF0) | 0x05;          // output upper nibble (RS=1, RW=0, E=1)
   LCD_PORT = temp;
   LCD_PORT = temp & ~0x04;      // E=0
   temp = (ch << 4) | 0x05;            // output lower nibble (RS=1, RW=0, E=1)
   LCD_PORT = temp;
   LCD_PORT = temp & ~0x04;      // E=0
}
 
static void checkbusy(){            
   _delay_ms(1);
}