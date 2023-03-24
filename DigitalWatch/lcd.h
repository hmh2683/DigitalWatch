#ifndef _LCD_H_
#define _LCD_H_

#define ALLCLR     0x01    // clear display
#define HOME       0x02    // locate cursor at home
#define LN21       0xc0    // locate cursor at the 2nd line
#define ENTMOD     0x06    // entry mode
#define FUNSET     0x28    // function set
#define DISP_ON    0x0c    // display on
#define DISP_OFF   0x08    // display off
#define CURSOR_ON  0x0e    // cursor on
#define CURSOR_OFF 0x0c    // cursor off
#define CURSOR_L   0x10    // move cursor to left
#define CURSOR_R   0x14    // move cursor to right
#define DISP_L     0x18    // move display to left
#define DISP_R     0x1c    // move display to right

void LcdInit(void);
void LcdCommand(char command);
void LcdMove(char line, char pos);
void LcdPutchar(char ch);
void LcdPuts(char * str);

#endif    