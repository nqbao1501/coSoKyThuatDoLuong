void changeMode() {
    modeFlag++;
    if (modeFlag > 3) modeFlag = 1;
    Serial.print("Mode: ");
    Serial.println(modeFlag);
}

void modeMaxMin(void)
{
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);
  if (tempMax < temp.temperature) tempMax = temp.temperature;
  if (tempMin > temp.temperature) tempMin = temp.temperature;

  lcd.setCursor(0, 0);
  lcd.print("Max: ");
  lcd.print(tempMax);
  lcd.print("C");

  lcd.setCursor(0, 1);
  lcd.print("Min: ");
  lcd.print(tempMin);
  lcd.print("C");

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
    return;
  }
  delay(SAMPLING_INTERVAL);
  lcd.clear();
}

void modeBinhThuong()
{
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);
  lcd.setCursor(0, 0);
  lcd.print("Nhiet do: ");
  lcd.print(temp.temperature);
  lcd.print("C");
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
    return;
  }
  delay(SAMPLING_INTERVAL);
  lcd.clear();
}

void modeXuLySoLieu(int mode) {
    // Sample data
    if (mode == BOTH)
    {
      lcd.setCursor(0, 0);
      lcd.print("Mode: ");
      lcd.setCursor(0, 1);
      lcd.print("Xu ly so lieu");
      delay(2000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Thu thap so lieu");
      float a_iTemp[NUM_SAMPLES];
      float a_iHumid[NUM_SAMPLES];
      for (int i = 0; i < NUM_SAMPLES; i++)
      {
        sensors_event_t humidity, temp;
        aht.getEvent(&humidity, &temp);
        a_iTemp[i] = temp.temperature;
        a_iHumid[i] = humidity.relative_humidity;
        Serial.println(a_iTemp[i]);
        Serial.println(a_iHumid[i]);
        delay(SAMPLING_INTERVAL);
      }

      // Calculate average of the array
      float meanTemp = average(a_iTemp, NUM_SAMPLES);
      float meanHumid = average(a_iHumid, NUM_SAMPLES);
      
      // Arrays to store errors and squared errors
      float errorsTemp[NUM_SAMPLES];
      float errorsHumid[NUM_SAMPLES];
      float squaredErrorsTemp[NUM_SAMPLES];
      float squaredErrorsHumid[NUM_SAMPLES];

      // Calculate errors (deviation from mean)
      for (int i = 0; i < NUM_SAMPLES; i++) {
          errorsTemp[i] = a_iTemp[i] - meanTemp;
          errorsHumid[i] = a_iHumid[i] - meanHumid;
      }

      // Calculate sum of absolute errors
      float sumAbsErrorsTemp = sumAbs(errorsTemp, NUM_SAMPLES);
      float sumAbsErrorsHumid = sumAbs(errorsHumid, NUM_SAMPLES);

      // Copy errors to squaredErrors and square them
      for (int i = 0; i < NUM_SAMPLES; i++) {
          squaredErrorsTemp[i] = errorsTemp[i];
          squaredErrorsHumid[i] = errorsHumid[i];        
      }
      squareElements(squaredErrorsTemp, NUM_SAMPLES);
      squareElements(squaredErrorsHumid, NUM_SAMPLES);

      // Calculate standard deviation
      float varianceTemp = sum(squaredErrorsTemp, NUM_SAMPLES) / (NUM_SAMPLES - 1);
      float varianceHumid = sum(squaredErrorsHumid, NUM_SAMPLES) / (NUM_SAMPLES - 1);
      float sigmaTemp = sqrt(varianceTemp);
      float sigmaHumid = sqrt(varianceHumid);
      
      // Calculate standard deviation of the mean (sigma_atb)
      float sigma_atbTemp = sigmaTemp / sqrt(NUM_SAMPLES);
      float sigma_atbHumid = sigmaHumid / sqrt(NUM_SAMPLES);
      // Output the result with ± symbol
      lcd.clear();
      char tempBuffer[16];
      char humidBuffer[16];

      // Format the strings
      sprintf(tempBuffer, "Temp:%.2f", meanTemp);
      sprintf(humidBuffer, "Hum:%.2f", meanHumid);

      // Display temperature
      lcd.setCursor(0, 0);
      lcd.print(tempBuffer);
      lcd.write((uint8_t)0);
      sprintf(tempBuffer, "%.2f", sigma_atbTemp * 3.1);
      lcd.print(tempBuffer);

      // Display humidity
      lcd.setCursor(0, 1);
      lcd.print(humidBuffer);
      lcd.write((uint8_t)0);
      sprintf(humidBuffer, "%.2f", sigma_atbHumid * 3.1);
      lcd.print(humidBuffer);    

      // Log to Serial
      Serial.printf("Temp: %.2f ± %.2f C\n", meanTemp, sigma_atbTemp * 3.1);
      Serial.printf("Hum: %.2f ± %.2f %%\n", meanHumid, sigma_atbHumid * 3.1);
    }
    else if (mode == TEMP)
    {
      float a_iTemp[NUM_SAMPLES];
      for (int i = 0; i < NUM_SAMPLES; i++)
      {
        lcd.setCursor(0, 0);
        lcd.print("Thu thap so lieu");
        sensors_event_t humidity, temp;
        aht.getEvent(&humidity, &temp);
        a_iTemp[i] = temp.temperature;
        lcd.setCursor(0, 1);
        lcd.print(temp.temperature);
        lcd.print("C");
        Serial.println(a_iTemp[i]);
        if (toggleLCDFlag)
        {
          lcdState = !lcdState;
          if (lcdState) {
              lcd.backlight();
          } else {
              lcd.noBacklight();
          }
          toggleLCDFlag = false;  
        }
        delay(SAMPLING_INTERVAL);
      }

      //
      float meanTemp = average(a_iTemp, NUM_SAMPLES);
      
      // Arrays to store errors and squared errors
      float errorsTemp[NUM_SAMPLES];
      float squaredErrorsTemp[NUM_SAMPLES];

      // Calculate errors (deviation from mean)
      for (int i = 0; i < NUM_SAMPLES; i++) {
          errorsTemp[i] = a_iTemp[i] - meanTemp;
      }

      // Calculate sum of absolute errors
      float sumAbsErrorsTemp = sumAbs(errorsTemp, NUM_SAMPLES);

      // Copy errors to squaredErrors and square them
      for (int i = 0; i < NUM_SAMPLES; i++) {
          squaredErrorsTemp[i] = errorsTemp[i];      
      }
      squareElements(squaredErrorsTemp, NUM_SAMPLES);

      // Calculate standard deviation
      float varianceTemp = sum(squaredErrorsTemp, NUM_SAMPLES) / (NUM_SAMPLES - 1);
      float sigmaTemp = sqrt(varianceTemp);
      
      // Calculate standard deviation of the mean (sigma_atb)
      float sigma_atbTemp = sigmaTemp / sqrt(NUM_SAMPLES);
      // Output the result with ± symbol
      lcd.clear();
      char tempBuffer[16];
      lcd.setCursor(0, 0);
      lcd.print("Xu ly 20 so: ");
      // Format the strings
      sprintf(tempBuffer, "%.2f", meanTemp, "C");

      // Display temperature
      lcd.setCursor(0, 1);
      lcd.print(tempBuffer);
      lcd.write((uint8_t)0);
      sprintf(tempBuffer, "%.2f", sigma_atbTemp * 3.1);
      lcd.print(tempBuffer);  
      lcd.print("C");

      // Log to Serial
      Serial.printf("Temp: %.2f ± %.2f C\n", meanTemp, sigma_atbTemp * 3.1);
    }  int currentButton1State = digitalRead(button1);
  if (toggleModeFlag) {
    changeMode();
    toggleModeFlag = false;
    return;
  }
  delay(5000);
  lcd.clear();
}
