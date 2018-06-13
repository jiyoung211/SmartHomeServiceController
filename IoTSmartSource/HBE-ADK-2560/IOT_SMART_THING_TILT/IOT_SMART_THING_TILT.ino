int pin_TILT = 2;

uint8_t Tilt_pre = 0;
uint8_t Tilt_Value = 0;
uint32_t Time_check_Tilt = 0;

void setup() {
  Serial.begin(115200);

  pinMode(pin_TILT, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(pin_TILT), TILT_ISR, CHANGE);
}

void loop() {
  if((Time_check_Tilt + 250) < millis())
  {
    if(Tilt_pre != Tilt_Value)
    {
      
      Tilt_pre = Tilt_Value;
      if(Tilt_Value == 1)
        Serial.println("Tilted");
      else
        Serial.println("Not Tilted");
    }
  }
}

void TILT_ISR(void)
{
  Tilt_Value = !digitalRead(pin_TILT);
  Time_check_Tilt = millis();
}

