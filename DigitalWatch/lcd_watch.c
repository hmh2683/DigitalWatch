#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "lcd.h"
 
#define    NO_KEY    0x00          // switch input
#define    KEY_IN      PINB        // switch input port
static unsigned char pin = NO_KEY; // former switch number (initial value = No switch pressed)
unsigned char key_input(void); 

/* date setting and display */
char *days[] = {"Mon", "Tue", "Wed", "Thr", "Fri", "Sat", "Sun"};
int monthdays[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
static volatile long int totaldays = 0;                                 // since 2001/01/01
int year=2001, month=0, day=0, week;                                    // since 2001/01/01
void total_days_2001(int y, int m, int d);                              // compute the number of days since 2001/01/01
void itotal_days_2001(long totaldays);                                  // compute year, month, day, date from total days
 
/* mode, watch, stopwatch time setting and display */
static volatile long int watch_total_sec;       // watch (elapsed time)
static volatile long int stop_total_sec;        // stopwatch (elapsed time)
static volatile char pmam[3] = "am";        
unsigned int mode = 0;           
unsigned int set_time = 0;                      // 0 : watch stop, 1 : watch go
unsigned int start_stop = 0;                    // 0 : stopwatch stop, 1 : stopwatch go

/* measure the temperature */
unsigned int adc_data;            
double temperature;

/* Overflow interrupt service routine */
ISR(TIMER0_OVF_vect) 
{            
    TCNT0 = 0;                                  // 1 interrupt for 1 sec (128/32768 x 256 = 1)
    if (start_stop == 1) stop_total_sec++;      // stopwatch go  
    if (set_time == 1) {                        // watch go
        watch_total_sec++;        
        if (watch_total_sec >= 86400)      
            watch_total_sec = 0;
    }
}
ISR(ADC_vect)
{
    adc_data = ADCW;                                    // read 10 bit
    temperature = ((double)adc_data)/1024.0 * 100.0;    // V(IN) = ADC / 1024 * V(REF) 
    ADCSR |= 0x40;                                      // start conversion
}

void main(void)
{
    char string1[20], string2[20];
    unsigned char sw_number;
    int itemp
    int hr, mn, sc;
    int dhr = 12;      
    int delay_count = 0;
 
    DDRF = 0xF7;        // set PF3(ADC3) as analog input(temp)
    DDRB = 0x00;        // set PB0(SW1)-PB7(SW8) as input
    DDRA = 0xFF;        // set PA0(LED1)-PA7(LED8) as output
    DDRD = 0xFF;        // set PD0(BUZZER) as output
 
    /* ADC setup */
    ADMUX = 0x03;        // AREF, right adjust, ADC channel 3
    ADCSR = 0xCE;        // enable, start conversion, interrupt enable, 1/64 
    
    /* Timer setup */
    ASSR |= (1<<AS0);                           // 32.768 KHz (2^15 Hz) 사용(외부클럭)
    TCCR0 = (1<<CS02) | (0<<CS01) | (1<<CS00);  // prescale = 1/128
    TCNT0 = 0;                                  // count 256
    TIMSK = (0<<OCIE0) | (1<<TOIE0);            // OCIE0 : disable (comparison match intterupt), TOTE0 : enable (overflow interrupt)
 
    sei();      // enable global interrupt
    LcdInit();              
 
    while(1)
    {         
        if (delay_count == 10){             
        itemp = (int)(temperature * 10.0 + 0.5);
        delay_count = 0;
        }
        delay_count++;
        PORTA = PINB;          

        /* read pressed switch */
        sw_number = key_input();       
        switch(sw_number)
        {
            case 0x08:  // switch 4 pressed (mode)
                mode = (mode+1) % 5;    
                break;
            
            case 0x04:  // switch 3 pressed
                if (mode == 0){         // set_date mode
                    year++;        
                    total_days_2001(year, month, day);
                }
                else if (mode == 1){    // set_time mode
                    watch_total_sec += 3600;    
                    if (watch_total_sec >= 86400)    
                        watch_total_sec -= 86400;
                }
                break;
            
            case 0x02:  // switch 2 pressed
                if (mode == 0){         
                    month = (month+1) % 12;
                    if ((year%4==0 && year%100!=0) || year%400==0) monthdays[1] = 29;
                    day = day % (monthdays[month]);
                    monthdays[1] = 28;
                    total_days_2001(year, month, day);
                }
                else if (mode == 1){    
                    watch_total_sec += 60;    
                    if ((watch_total_sec % 3600) < 60) 
                        watch_total_sec -= 3600;
                }
                else if (mode == 4){    // stopwatch mode (reset)
                    start_stop = 0;     
                    stop_total_sec = 0;
                }
                break;
            case 0x01:  // switch 1 pressed
                if (mode == 0){         
                    if ((year%4==0 && year%100!=0) || year%400==0) monthdays[1] = 29;
                    day = (day+1) % (monthdays[month]);
                    monthdays[1] = 28;
                    total_days_2001(year, month, day);
                }
                else if (mode == 1) set_time ^= 1;      // set_time mode (start/stop toggle)
                else if (mode == 4) start_stop ^= 1;    // stopwatch mode (start/stop toggle)
                break;
            default:
                break;
        }

        /* watch display */
        if (mode == 0 || mode == 1 || mode == 2 || mode == 3) 
        {
            itotal_days_2001(totaldays);   
            
            /* 00:00 ~ 11:59 am, 12:00 ~ 23:59 pm  */
            if (watch_total_sec >= 43200) pmam[0] = 'p';        
            else pmam[0] = 'a';

            /* 12:00:00 ~ 11:59:59 */ 
            hr = watch_total_sec/3600;
            dhr = hr;               
            if (hr>12) dhr = hr-12;
            if (hr == 0) dhr = 12;
            
            mn = (watch_total_sec % 3600)/60;
            sc = watch_total_sec - hr*3600 - mn*60;
            if (mn < 10) string2[8] = '0';
            if (sc < 10) string2[13] = '0';

            /* buzzer on for 5 sec at every hour */
            if ((mn == 0) && (sc <= 5) && (mode == 2 || mode == 3)) PORTD = 0x00;   
            else PORTD = 0xFF; 

            switch(mode)
            {
                case 0:
                    sprintf(string1, "SET DATE        ");
                    sprintf(string2, "%4d/%2d/%2d  %s ", year, month+1, day+1, days[week]);
                    if (month < 9) string2[5] = '0';
                    if (day < 9) string2[8] = '0';
                    break;
                case 1:
                    sprintf(string1, "SET TIME        ");
                    sprintf(string2, "%s %2d : %2d : %2d ", pmam, dhr, mn, sc);
                    if (mn < 10) string2[8] = '0';
                    if (sc < 10) string2[13] = '0';
                    break;
                case 2:
                    sprintf(string1, "%4d/%2d/%2d  %s ", year, month+1, day+1, days[week]);
                    if (month < 9) string1[5] = '0';
                    if (day < 9) string1[8] = '0';
                    sprintf(string2, "%s %2d : %2d : %2d ", pmam, dhr, mn, sc);
                    if (mn < 10) string2[8] = '0';
                    if (sc < 10) string2[13] = '0';
                    break;
                case 3:
                    sprintf(string1, "%s    %d.%d %cC ", "TIME", itemp/10, itemp % 10, 0xDF);
                    sprintf(string2, "%s %2d : %2d : %2d ", pmam, dhr, mn, sc);
                    if (mn < 10) string2[8] = '0';
                    if (sc < 10) string2[13] = '0';
                    break;
                default:
                    break;
            }
        }

        /* stopwatch display */
        else        
        {                                    
            hr = stop_total_sec/3600;
            mn = (stop_total_sec % 3600)/60;
            sc = stop_total_sec - hr *3600 - mn*60;
 
            sprintf(string2, "   %2d : %2d : %2d ", hr, mn, sc);
            if (hr < 10) string2[3] = '0';
            if (mn < 10) string2[8] = '0';
            if (sc < 10) string2[13] = '0';
            sprintf(string1, "%s    %d.%d %cC ", "STOP", itemp/10, itemp % 10, 0xDF);
        }

        LcdMove(0,0);
        LcdPuts(string1);          
        LcdMove(1,0);
        LcdPuts(string2);            
    }
}
 
/* 입력: 연(y), 월(m), 일(d) */  
/* 출력: 경과일(totaldays), 요일(week) */
void total_days_2001(int y, int m, int d)
{
    totaldays = (y-2001)*365 + (y-2001)/4 - (y-2001)/100 + (y-2001)/400;  
    if(((y%4==0) && (y%100!=0)) || (y%400==0)) monthdays[1]=29;          
    while (m > 0){
        totaldays += monthdays[m-1];
        m--;
    }
    totaldays += d;
    week = totaldays % 7;
}

/* 입력: 경과일(totaldays) */
/* 출력: 연(year), 월(month), 일(day) */
void itotal_days_2001(long totaldays)
{    
    int y, m, d, t;
    y = 2001, m = 0, d = 0;   
    t = totaldays;      // totaldays = 0, 2001년 1월 1일
    
    while (t >= 0){               
        if ((y%4==0 && y%100!=0) || y%400==0) t -= 366;
        else t -= 365;
        y++;
    }
    y--;    // 조정                   
    if ((y%4==0 && y%100!=0) || y%400==0) t += 366;
    else t += 365;
 
    if ((y%4==0 && y%100!=0) || y%400==0) monthdays[1] = 29;  
    while (t >= monthdays[m]){          
        t -= monthdays[m];
        m++;
    }
    monthdays[1] = 28;
    
    day = t;            // day = 0, 1일            
    month = m;          // month = 0, 1월  
    year = y;
}

unsigned char key_input(void)
{
    unsigned char in, in1;
    in = ~KEY_IN;       // read key_code
    
    while(1){
        in1 = ~KEY_IN;  // read key_code one more time
        if (in == in1) break;
        in = in1;
    }
    if(!in){            // no key
        pin = 0;
        return 0;
    }
    if (pin == in)      // same key pressed continuously
        return 0;               
    
    pin = in;
    return in;          // return key code
}