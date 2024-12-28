#include <Adafruit_AHTX0.h>
#include <math.h>
#include <stdio.h>
#include <LiquidCrystal_I2C.h>

#define SAMPLING_INTERVAL 2000
#define NUM_SAMPLES 20
#define TEMP        0
#define BOTH        1

#define button1     4
#define button2     19

Adafruit_AHTX0 aht;

int lcdColumns = 16;
int lcdRows = 2;
int lcdState = HIGH;  
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);

volatile bool toggleLCDFlag = false;
volatile bool toggleModeFlag= false;
bool button1State = HIGH;                
          
int modeFlag = 1;
volatile unsigned long lastInterruptTimeButton1 = 0;
volatile unsigned long lastInterruptTimeButton2 = 0;
unsigned long previousMillis = 0;

float tempMin;
float tempMax;

void setup() {
  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);

  Serial.begin(115200);
  if (!aht.begin()) {
    Serial.println("Could not find AHT? Check wiring");
    while (1) delay(10);
  }
  Serial.println("AHT10 or AHT20 found");
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);

  tempMin = tempMax = temp.temperature;

  lcd.init();                     
  lcd.backlight();
    // Define the custom ± symbol
  byte plusMinus[8] = {
      0b00000,
      0b00100,
      0b00100,
      0b11111,
      0b00100,
      0b00100,
      0b11111,
      0b00000
  };
  lcd.createChar(0, plusMinus); 
  attachInterrupt(digitalPinToInterrupt(button1), changeModeInterrupt, FALLING);  
  attachInterrupt(digitalPinToInterrupt(button2), toggleLCDInterrupt, FALLING);
}

void loop() {
  if (toggleLCDFlag)
  {
    lcdState = !lcdState;
    if (lcdState) {
        lcd.backlight();
    } else {
        lcd.noBacklight();
    }
    toggleLCDFlag = false; // Reset the flag    
  }

  if (toggleModeFlag) {
    changeMode();
    toggleModeFlag = false;
  }

  if (modeFlag == 1) modeBinhThuong();
  if (modeFlag == 2) modeMaxMin();
  if (modeFlag == 3)
  {
    lcd.clear();
    modeXuLySoLieu(TEMP);
  }
}

void toggleLCDInterrupt() {
  unsigned long interruptTime = millis();
  if (interruptTime - lastInterruptTimeButton2 > 200) { 
      toggleLCDFlag = true; 
      lastInterruptTimeButton2 = interruptTime;
  }
}
void changeModeInterrupt()
{
  unsigned long interruptTime = millis();
  if (interruptTime - lastInterruptTimeButton1 > 200) { 
      toggleModeFlag = true; 
      lastInterruptTimeButton1 = interruptTime;
  }  
}

