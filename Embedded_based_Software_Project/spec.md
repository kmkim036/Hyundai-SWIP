# Hardware
## Input
- SW 1: GPIO (Polling, Interrupt)  
- SW 2: GPIO (Polling, Interrupt)  
- Potentiometer: ADC (Polling)  
- Light Sensor: ADC (Polling)  
- LM35(Temperature): ADC (Polling)
- Ultrasonic Sensor: GPIO  

## Output
- LED Red: GPIO, PWM  
- LED Blue: GPIO, PWM  
- RGB LED: GPIO(ADC), PWM   
- Buzzer: PWM  



# Function

- 초음파센서 => 거리 측정 => 부저 + LED 점등 주기 제어

  GPIO input	=>	PWM(Buzzer), GPIO(LED), Timer

- 조도센서 => 밝기 측정 => LED 제어

  ADC input	=>	PWM(LED) or GPIO(LED) 

- 스위치 => 디지털 입력 => 시동 상태 제어

  Digital input (interrupt) => ADC(RGB LED)

- 가변저항 => 에어컨 다이얼 조절 => RGB LED 제어

  ADC input	=>	ADC(RGB LED)



# Sub Item

## 게임기

### 1. 두더지 잡기 

SWITCH, LED

### 2. 무궁화 꽃이 피었습니다 

초음파센서, 부저

### 3. 제로섬게임 (깃발뺏기)

가변저항, RGB LED

### 4. 찾기

조도센서, RGB LED



