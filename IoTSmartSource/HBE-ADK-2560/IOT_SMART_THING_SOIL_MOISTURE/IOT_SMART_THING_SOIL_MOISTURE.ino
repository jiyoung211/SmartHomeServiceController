int pin_SOIL_MOISTURE = A5;

void setup() {
  Serial.begin(115200);

  pinMode(pin_SOIL_MOISTURE, INPUT);
}

void loop() {
  uint16_t Sensor_Data, ADC_data;

  ADC_data = analogRead(pin_SOIL_MOISTURE);

  Sensor_Data = ADC_data / 9.7; // ADC_Data * 100 / 970 -> [%]

  Serial.print("ADC Data : ");
  Serial.print(ADC_data);
  Serial.print(", ");
  Serial.print(Sensor_Data);
  Serial.println("[%]");
  delay(500);
}
