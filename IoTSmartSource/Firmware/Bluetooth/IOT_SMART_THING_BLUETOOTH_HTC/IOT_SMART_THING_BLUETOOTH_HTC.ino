#include <SPP.h>
#include <SPI.h>
#include <MsTimer2.h>
#include <Wire.h>
#include <SHT2x.h>

#define Group_ID  10

#define TX_Sensor_Packet_length 23

int pin_CDS = A2;

uint8_t TX_Sensor_data[TX_Sensor_Packet_length] = {0x76, 0x30, };
uint8_t TX_Sensor_flag = 1;
uint16_t ADC_Data[20] = {3, 45, 75, 90, 109, 123, 135, 144, 157, 169, 178, 241, 281, 322, 346, 365, 393, 402, 409, 422};
uint16_t LUX_Data[20] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100};

char Bluetooth_Name[] = "Arduino--";

USB Usb;
//USBHub Hub1(&Usb); // Some dongles have a hub inside

BTD Btd(&Usb); // You have to create the Bluetooth Dongle instance like so
/* You can create the instance of the class in two ways */
//SPP SerialBT(&Btd); // This will set the name to the defaults: "Arduino" and the pin to "0000"
SPP SerialBT(&Btd, Bluetooth_Name, "1234"); // You can also set the name and pin like so

void setup() {
  // put your setup code here, to run once:
  uint8_t i;
  
  Serial.begin(115200);

  for(i=2; i<(TX_Sensor_Packet_length-1); i++)
  {
    TX_Sensor_data[i] = 0xFF;
  }
  TX_Sensor_data[TX_Sensor_Packet_length-1] = 0x3E;
  
  pinMode(pin_CDS, INPUT);

  Wire.begin();

  Bluetooth_Name[7] = Group_ID/10 + '0';
  Bluetooth_Name[8] = Group_ID%10 + '0';

  MsTimer2::set(5000, Sensor_Read_ISR);
  MsTimer2::start();

  //USB init
  if (Usb.Init() == -1) {
    Serial.print(F("\r\nOSC did not start"));
    while(1); //halt
  }
  Serial.print(F("\r\nSPP Bluetooth Library Started"));
}

void loop() {
  // put your main code here, to run repeatedly:
  uint16_t LUX = 0;
  int8_t Temp = 0;
  uint8_t i, Humi = 0;
  
  Usb.Task(); // The SPP data is actually not send until this is called, one could call SerialBT.send() directly as well
  if(SerialBT.connected != 0)
  {
    if(TX_Sensor_flag == 1)
    {
      Humi = SHT2x.readRH();
      Temp = SHT2x.readT();
      
      LUX = ADC_TO_LUX(analogRead(pin_CDS));
      
      TX_Sensor_data[3] = Temp;
      TX_Sensor_data[4] = Humi;
      TX_Sensor_data[7] = LUX;

      SerialBT.write(TX_Sensor_data, TX_Sensor_Packet_length);
      
      Serial.println("\n\rTX Packet :");
      for(i=0; i<TX_Sensor_Packet_length; i++)
      {
        Serial.print(TX_Sensor_data[i], HEX);
        Serial.write(' ');
      }
      
      TX_Sensor_flag = 0;
    }
  }
}

void Sensor_Read_ISR(void)
{
  TX_Sensor_flag = 1;
}

uint16_t ADC_TO_LUX(uint16_t ADC_data)
{
  uint16_t Lux = 0, i;
  for(i=0; i<20; i++)
  {
    if(ADC_data < ADC_Data[i])
    {
      break;
    }
  }
  if(i < 2)
  {
    Lux = 0;
  }
  else if(i < 11)
  {
    Lux = LUX_Data[i-1];
  }
  else if(i > 19)
  {
    Lux = 100;
  }
  else
  {
    Lux = LUX_Data[i-1] + (LUX_Data[i]-LUX_Data[i-1]) * (ADC_data - ADC_Data[i-1]) / (ADC_Data[i]-ADC_Data[i-1]);
  }
  
  return Lux;
}
