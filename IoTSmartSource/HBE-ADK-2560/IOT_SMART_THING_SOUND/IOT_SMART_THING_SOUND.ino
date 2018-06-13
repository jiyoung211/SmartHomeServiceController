int pin_SOUND = 3;

uint8_t Sound_flag = 0;
uint32_t Time_check_Sound = 0;

void setup() {
  Serial.begin(115200);

  pinMode(pin_SOUND, INPUT);

  attachInterrupt(digitalPinToInterrupt(pin_SOUND), SOUND_ISR, RISING);
}

void loop() {
  if((Sound_flag == 1) && (Time_check_Sound < millis()))
  {
    Sound_flag = 0;
    Serial.println("None sensing");
  }
}

void SOUND_ISR(void)
{
  if(Sound_flag != 1)
  {
    Sound_flag = 1;
    Serial.println("Sensing");
  }
  Time_check_Sound = millis()+2000;
}

