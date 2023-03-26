# DigitalWatch
* 개발 기간 : 2020.09 ~ 2020.11  
* 개발 인원 : 2 
* 개발 환경 : AVR Studio 4 (Win AVR Compiler)
* 개발 언어 : C
* 개발 목표  

<br/> <br/>

## Microcontroller
<a href="#"><img src="https://github.com/hmh2683/DigitalWatch/blob/main/image/mcu.png" width="400px" height="300px"></a> 
* Board : M128RS1
* Core : AVR ATmega128 
* Clock : 16MHz
* RTC : 32.768KHz

<br/> <br/>

## Pinout Configuration
### System Core
* GPIO -> INPUT -> UP_BUTTON, SET_BUTTON, DOWN_BUTTON, START_SW
* GPIO -> OUTPUT -> TEMP_DATA, RELAY, BUTTON_LED, START_LED, FND_RCLK

* TIM0 -> TEMPSENSOR
  * Prescaler : 71
  * Counter Period : 65535
* TIM3 -> FND
  * Prescaler : 71
  * Counter Period : 99

<br/> <br/>

## Code Review
### Main
* SelectButton 함수에서 버튼 입력에 대한 OLED, LED, USART 를 제어합니다. 
* 온도 변환 상태를 확인하고 현재온도를 반환합니다. 
* 스위치가 ON 이라면, Relay는 GetTemp 함수의 반환 값과 SelectButton 함수 내 전역 변수값을 비교하며 작동합니다. 
* 스위치가 OFF 라면, Relay는 값에 상관없이 작동하지 않습니다.
```C
float temperature = 0.0;
while (1)
{
	SelectButton();

	if (!GetConvertState())
		StartConverting();
	CheckConverting();

	if (!GetConvertState()) // 온도변환 완료 시
	{
		temperature = GetTemp();
		CheckSwitch((int)temperature);
	}
}
```

<br/>

### Interrupt
* 다수의 입력을 방지하기 위해 HAL_GetTick 함수를 사용하고 CLICK_TIME을 200으로 설정하며 0.2s 마다 실행합니다. (1 Tick = 1ms)
```C
void EXTI0_IRQHandler(void) 
{
  HAL_GPIO_EXTI_IRQHandler(PB0_TEMP_UP_BUTTON_Pin);

	if (HAL_GetTick() - before_time > CLICK_TIME) 
		button_up = 1;
	
	before_time = HAL_GetTick();
}
```

<br/>

* TIM3 prescaler: 72, period: 100으로 설정하여 100us 마다 Timer를 실행합니다. ((72 / 72M) * 100 = 100u)
* 센서 초기화 상태와 OneWire 실행 상태를 확인하고 참이라면 현재온도를 FND에 표시합니다.
```C
void TIM3_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&htim3);
  
  if (GetSensorInitState() && !GetBusy())
	DisplayTemp((int)GetCurrentTemp() * 10);
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
