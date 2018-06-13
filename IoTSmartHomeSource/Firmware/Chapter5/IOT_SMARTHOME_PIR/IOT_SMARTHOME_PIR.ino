int pin_PIR_INT = 3;
int PIR_flag = 0, PIR_value = 0,   PIR_value_pre = 0;
uint32_t time_check = 0;

int myPinCallback(uint32_t ulPin)
{
    PIR_value = 1;
    time_check = millis() + 500;
    return 0;  // don't exit   RFduino_ULPDelay
}

void setup() {
    Serial.begin(9600);

    pinMode(pin_PIR_INT, INPUT);
  RFduino_pinWakeCallback(pin_PIR_INT, HIGH, myPinCallback);

    Serial.println("\r\nstart");
}

void loop(){

    if(PIR_value)
  {
      if((time_check < millis()) && (digitalRead(pin_PIR_INT)==0))
      {
        PIR_value = 0;
      }
  }
    if(PIR_value_pre != PIR_value)
  {
      PIR_value_pre = PIR_value;
      Serial.print("PIR ");
      if(PIR_value)
        Serial.println("ON");
      else
        Serial.println("OFF");
  }
}
