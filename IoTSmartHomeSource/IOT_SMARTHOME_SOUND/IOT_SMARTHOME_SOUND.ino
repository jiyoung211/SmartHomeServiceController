int pin_Sound_INT = 2;
int Sound_count = 0, Sound_value = 0;
uint32_t time_check = 0;

int myPinCallback(uint32_t ulPin)
{
  Sound_count++;
  return 0;
}

void setup() {
  Serial.begin(9600);
  
  pinMode(pin_Sound_INT, INPUT);
  RFduino_pinWakeCallback(pin_Sound_INT, HIGH, myPinCallback);
  
  Serial.println("\r\nstart");
}

void loop() {
  if((Sound_count > 1))
  {
    Sound_value = 1;
    Sound_count = 0;
    time_check = millis() + 1000;
    Serial.println("Sound ON");
  }
  
  if(Sound_value)
  {
    if((time_check < millis()) && (digitalRead(pin_Sound_INT)==0))
    {
      Sound_value = 0;
      Serial.println("Sound OFF");
    }
  }
}
