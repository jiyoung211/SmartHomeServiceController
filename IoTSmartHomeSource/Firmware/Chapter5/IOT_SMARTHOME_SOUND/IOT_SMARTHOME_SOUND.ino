int pin_SOUND_INT = 2;
int SOUND_count = 0, SOUND_value = 0;
uint32_t time_check = 0;

int myPinCallback(uint32_t ulPin)
{
    SOUND_count++;
    return 0;  // don't exit   RFduino_ULPDelay
}

void setup() {
    Serial.begin(9600);

    pinMode(pin_SOUND_INT, INPUT);
    RFduino_pinWakeCallback(pin_SOUND_INT, HIGH, myPinCallback);
    Serial.println("\r\nstart");
}
void loop(){
    if((SOUND_count > 1))
  {
      SOUND_value = 1;
      SOUND_count = 0;
      time_check = millis() + 1000;
      Serial.println("SOUND ON");
  }
    if(SOUND_value)
  {
      if((time_check < millis()) && (digitalRead(pin_SOUND_INT)==0))
      {
        SOUND_value = 0;
        Serial.println("SOUND OFF");
      }
  }
}
