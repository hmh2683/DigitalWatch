# Digital watch
* 개발 기간 : 2020.09 ~ 2020.11  
* 개발 인원 : 2 명
* 개발 환경 : AVR Studio 4 (Win AVR Compiler)
* 개발 언어 : C
* 개발 목표  
  * SET DATE 및 SET TIME 모드에서 SW3, SW2, SW1을 이용하여 연/월/일, 시:분:초를 설정합니다.  
  * SW4를 이용하여 모드를 전환하고, LCD에서 날짜, 시간, 온도를 표시합니다.
  * STOPWATCH 모드에서 SW1을 누르면 카운트를 시작 및 중지하고, SW2를 누르면 초기화합니다.

<br/> <br/>

## Microcontroller
<a href="#"><img src="https://github.com/hmh2683/DigitalWatch/blob/main/image/mcu.png" width="500px" height="400px"></a> 
* Board : M128RS1
* Core : AVR ATmega128 
* Clock : 16MHz
* RTC : 32.768KHz

<br/> <br/>

## Pinout configuration
<a href="#"><img src="https://github.com/hmh2683/DigitalWatch/blob/main/image/circuit.png" width="500px" height="400px"></a> 
### System core
* (LED) PA0-PA7 : LED1-LED8
* (SW) PB0-PB7 : SW1-SW8 
* (LCD) PC0 : RS, PC1 : RW, PC2 : EN, PC4-PC7 : D4-D7 
* (Buzzer) PD0 : BUZZER
* (Sensor) PF3 : ADC3(LM35DZ), VCC : AREF

### Interrupt
* TIMER
  * Clock : 32.768 KHz
  * Prescaler : 1/128
  * Count period : 256
  * Overflow interrupt
 
* ADC
   * ADC channel : 3
   * ADLAR : Right
   * Prescaler : 1/64

<br/> <br/>

## Code review
### Main
* 스위치에 따라 연/월/일, 시:분:초를 증가시키고, 경과일(totaldays)과 경과시간(total_sec)을 반환합니다.  
* 경과일과 경과시간에서 연/월/일, 요일, 시:분:초 데이터를 추출합니다 
* 모드 상태에 따라 연/월/일, 요일, 시:분:초, 온도 등을 string1, string2 배열 안에 저장합니다.
* lcd.h 포함하고 관련 함수를 사용하여 string1, string2 배열을 LCD 모듈에 출력합니다.

```C
while(1)
{         	           
    itemp = (int)(temperature * 10.0 + 0.5);                    

    sw_number = key_input();       
    switch(sw_number) { total_days_2001(year, month, day); } 
    
    if (mode == 0 || mode == 1 || mode == 2 || mode == 3) { itotal_days_2001(totaldays); } // watch
    else { } // stopwatch
    
    LcdMove(0,0);
    LcdPuts(string1);          
    LcdMove(1,0);
    LcdPuts(string2); 
}
```

<br/>

### Function
* 키가 연속적으로 입력되는 것을 방지합니다. 
```C
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
```

<br/>

### Interrupt
* 타이머 설정을 통해 1초에 1번 인터럽트가 발생합니다.
```C
ASSR |= (1<<AS0);                           // 32.768 KHz (2^15 Hz) 
TCCR0 = (1<<CS02) | (0<<CS01) | (1<<CS00);  // prescale = 1/128
TCNT0 = 0;                                  // count 256
TIMSK = (0<<OCIE0) | (1<<TOIE0);            // enable overflow interrupt

ISR(TIMER0_OVF_vect) 
{            
    TCNT0 = 0;                              // 1 interrupt for 1 sec (128/32768 x 256 = 1)
    if (start_stop == 1) stop_total_sec++;     
    if (set_time == 1) {                     
        watch_total_sec++;        
        if (watch_total_sec >= 86400)      
            watch_total_sec = 0;
    }
}
```

<br/>

* ADC 결과를 ADCW(10bit)에 저장하고, 해당 값을 보기 쉬운 데이터(온도)로 변환합니다. (Vcc-AREF 연결)
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
<a href="#"><img src="https://github.com/hmh2683/DigitalWatch/blob/main/image/setdate.png" width="400px" height="300px"></a>
<a href="#"><img src="https://github.com/hmh2683/DigitalWatch/blob/main/image/settime.png" width="400px" height="300px"></a>
<a href="#"><img src="https://github.com/hmh2683/DigitalWatch/blob/main/image/datetime.png" width="400px" height="300px"></a>
<a href="#"><img src="https://github.com/hmh2683/DigitalWatch/blob/main/image/temptime.png" width="400px" height="300px"></a>
<a href="#"><img src="https://github.com/hmh2683/DigitalWatch/blob/main/image/start.png" width="400px" height="300px"></a>
<a href="#"><img src="https://github.com/hmh2683/DigitalWatch/blob/main/image/stop.png" width="400px" height="300px"></a>
