int pin_RAIN_STEAM = A4;

void setup() {
  Serial.begin(115200);

  pinMode(pin_RAIN_STEAM, INPUT);
}

void loop() {
  uint16_t Sensor_Data, ADC_data;

  ADC_data = analogRead(pin_RAIN_STEAM);

  Sensor_Data = ADC_data / 9.7; // ADC_Data * 100 / 970 -> [%]

  Serial.print("ADC Data : ");
  Serial.print(ADC_data);
  Serial.print(", ");
  Serial.print(Sensor_Data);
  Serial.println("[%]");
  delay(500);
}
