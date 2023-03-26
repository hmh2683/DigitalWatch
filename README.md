# DigitalWatch
* 개발 기간 : 2020.09 ~ 2020.11  
* 개발 인원 : 2 
* 개발 환경 : AVR Studio 4 (Win AVR Compiler)
* 개발 언어 : C
* 개발 목표  
  * 차량은
  * 차량은 
  * 차량은 

<br/> <br/>

## Microcontroller
<a href="#"><img src="https://github.com/hmh2683/DigitalWatch/blob/main/image/mcu.png" width="400px" height="300px"></a> 
* Board : M128RS1
* Core : AVR ATmega128 
* Clock : 16MHz
* RTC : 32.768KHz

<br/> <br/>

## Pinout Configuration
* LED -> OUTPUT -> 2
* DCMotor -> OUTPUT -> 8, 9, 10, 11
* ServoMotor -> OUTPUT -> 12, 13 
* ColorSensor -> OUTPUT -> A0, A1, A2, A3
* ColorSensor -> INPUT -> 3

<br/> <br/>

## Code Review
* RGB(0~255)를 추출하기 위해 S0 HIGH, S1 LOW 로 설정하여 주파수를 20% 크기로 출력합니다. (0%, 2%, 20%, 100%)
* S2, S3 상태에 따라 RGB 색상의 주파수를 출력합니다. (S2:LOW S3:HIGH -> BULE, S2 S3:HIGH -> GREEN)  
* pulseIn, map, constrain 함수를 통해 주파수를 0~255 값의 범위로 반환합니다.

```C
  digitalWrite(s0,HIGH);  
  digitalWrite(s1,LOW);
  
  digitalWrite(s2,LOW);               
  digitalWrite(s3,LOW);
  
  red_color = pulseIn(out_pin, LOW);
  red_color = map(red_color,25,72,255,0);  
  red_color = constrain(red_color,0,255);
```

<br/>

* 두 개의 서보 모터는 12번, 13번 핀으로 구성합니다.
* 서보 12는 75도와 50도를 설정하여 물체를 수직으로 올리거나 내립니다.
* 서보 13은 75도 및 110도 설정으로 물체를 수평으로 잡거나 놓습니다.

```C
  TiltServo.attach(12);  
  GripServo.attach(13);

  GripServo.write(75); 
  delay(200);
  TiltServo.write(75);
  delay(100);
  
  TiltServo.write(50);  
  delay(100);
  GripServo.write(110); 
  delay(200);
```

<br/> <br/>

## Result
<a href="#"><img src="https://github.com/hmh2683/CarryingVehicleSystem/blob/main/image/motor.png" width="400px" height="300px"></a>
<a href="#"><img src="https://github.com/hmh2683/CarryingVehicleSystem/blob/main/image/sensor.png" width="400px" height="300px"></a>
<a href="#"><img src="https://github.com/hmh2683/CarryingVehicleSystem/blob/main/image/result2.png" width="400px" height="300px"></a>
<a href="#"><img src="https://github.com/hmh2683/CarryingVehicleSystem/blob/main/image/result.png" width="400px" height="300px"></a>

<br/> <br/>

## Realization
* 프로젝트의 설계, 구현, 테스트 단계를 경험하며 실무능력을 강화하였습니다.
* 컬러센서, 서보모터 등 하드웨어의 데이터시트와 오픈소스를 분석하며 기본동작을 구현하였습니다.
* 차량에 탑재하는 하드웨어 및 소프트웨어를 목적에 맞도록 통합하였습니다.
