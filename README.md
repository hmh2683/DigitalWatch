# Digital Watch
* 개발 기간 : 2020.09 ~ 2020.11  
* 개발 인원 : 2 
* 개발 환경 : AVR Studio 4 (Win AVR Compiler)
* 개발 언어 : C
* 개발 목표  
  * SET DATE 및 SET TIME 모드에서 SW3, SW2, SW1을 이용하여 연/월/일, 시:분:초를 설정합니다.  
  * SW4를 이용하여 모드를 전환하고, LCD에서 날짜, 시간, 온도를 표시합니다.
  * STOPWATCH 모드에서 SW1을 누르면 카운트를 시작 및 중지하고, SW2를 누르면 초기화합니다.

<br/> <br/>

## Microcontroller
<a href="#"><img src="https://github.com/hmh2683/DigitalWatch/blob/main/image/mcu.png" width="400px" height="300px"></a> 
* Board : M128RS1
* Core : AVR ATmega128 
* Clock : 16MHz
* RTC : 32.768KHz

<br/> <br/>

## Pinout Configuration
* LED -> PA0-PA7 : LED1-LED7
* LCD -> PC0 : RS, PC1 : RW, PC2 : EN, PC4-PC7 : D4-D7
* BUZZER -> PD0 : BUZZER
* SW -> PB0-PB3 : SW1-SW4 

<br/> <br/>

## Code Review
### Main
* 
```C
while(1)
{         	           
    itemp = (int)(temperature * 10.0 + 0.5);                    

    sw_number = key_input();       
    switch(sw_number) { total_days_2001(year, month, day); }
    
    if (mode == 0 || mode == 1 || mode == 2 || mode == 3) { itotal_days_2001(totaldays); }
    else { } //stopwatch
    
    LcdMove(0,0);
    LcdPuts(string1);          
    LcdMove(1,0);
    LcdPuts(string2); 
}
```
<br/>

### Function
```C
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

```

<br/>

```C

```

<br/>

```C

```

<br/>

### Interrupt
* TIMER
```C
ASSR |= (1<<AS0);                           // 32.768 KHz (2^15 Hz) 사용(외부클럭)
TCCR0 = (1<<CS02) | (0<<CS01) | (1<<CS00);  // prescale = 1/128
TCNT0 = 0;                                  // count 256
TIMSK = (0<<OCIE0) | (1<<TOIE0);            // OCIE0 : disable (comparison match intterupt), TOTE0 : enable 
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
```

<br/>

* ADC
```C
ADMUX = 0x03;        // AREF, right adjust, ADC channel 3
ADCSR = 0xCE;        // enable, start conversion, interrupt enable, 1/64 
ISR(ADC_vect)
{
    adc_data = ADCW;                                    // read 10 bit
    temperature = ((double)adc_data)/1024.0 * 100.0;    // V(IN) = ADC / 1024 * V(REF) 
    ADCSR |= 0x40;                                      // start conversion
}
```

<br/> <br/>

## Result
<a href="#"><img src="https://github.com/hmh2683/DigitalWatch/blob/main/image/setdate.png" width="500px" height="400px"></a>
<a href="#"><img src="https://github.com/hmh2683/DigitalWatch/blob/main/image/settime.png" width="500px" height="400px"></a>
<a href="#"><img src="https://github.com/hmh2683/DigitalWatch/blob/main/image/datetime.png" width="500px" height="400px"></a>
<a href="#"><img src="https://github.com/hmh2683/DigitalWatch/blob/main/image/temptime.png" width="500px" height="400px"></a>
<a href="#"><img src="https://github.com/hmh2683/DigitalWatch/blob/main/image/start.png" width="500px" height="400px"></a>
<a href="#"><img src="https://github.com/hmh2683/DigitalWatch/blob/main/image/stop.png" width="500px" height="400px"></a>
