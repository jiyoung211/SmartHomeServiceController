int pin_DUST_LED = 8;
int pin_DUST_OUT = A7;

void setup() {
  Serial.begin(115200);
  
  pinMode(pin_DUST_LED, OUTPUT);
  pinMode(pin_DUST_OUT, INPUT);
  digitalWrite(pin_DUST_LED, HIGH);
}

void loop() {
  uint16_t ADC_Data = 0, Sensor_Data = 0;
  ADC_Data = Dust_Read();

  Sensor_Data = Dust_tr(ADC_Data); // (ADC data -> [ug/m3])

  Serial.print("ADC_data : ");
  Serial.print(ADC_Data);
  Serial.print(", dust : ");
  Serial.print(Sensor_Data);
  Serial.println("[ug/m3]");
  delay(500);
}

//Read Dust Sensor data
uint16_t Dust_Read(void)
{
  uint16_t Sensor_data;
  float tmp = 0;
  digitalWrite(pin_DUST_LED, LOW);
  delayMicroseconds(280);
  Sensor_data = analogRead(pin_DUST_OUT);
  delayMicroseconds(40);
  digitalWrite(pin_DUST_LED, HIGH);
  //delayMicroseconds(9680);
  return Sensor_data;
}

// ADC data -> [ug/m3]
float Dust_tr(uint16_t ADC_data)
{
  float Dust = ADC_data;
  Dust = Dust*5.0/1023.0/5.95 - 0.1;

  return Dust*1000;
}

