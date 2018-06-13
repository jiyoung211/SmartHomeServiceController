int pin_PIR = 17;

uint8_t PIR_flag = 0;
uint32_t Time_check_PIR = 0;

void setup() {
  Serial.begin(115200);

  pinMode(pin_PIR, INPUT);

  attachInterrupt(digitalPinToInterrupt(pin_PIR), PIR_ISR, RISING);
}

void loop() {
  if((PIR_flag == 1) && (Time_check_PIR < millis()))
  {
    PIR_flag = 0;
    Serial.println("None sensing");
  }
}

void PIR_ISR(void)
{
  if(PIR_flag != 1)
  {
    PIR_flag = 1;
    Serial.println("Sensing");
  }
  Time_check_PIR = millis()+2000;
}

