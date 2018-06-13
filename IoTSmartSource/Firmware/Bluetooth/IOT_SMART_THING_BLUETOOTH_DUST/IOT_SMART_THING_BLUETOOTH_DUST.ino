#include <SPP.h>
#include <SPI.h>
#include <MsTimer2.h>

#define Group_ID  10

#define TX_Sensor_Packet_length 23

int pin_DUST_LED = 8;
int pin_DUST_OUT = A7;

uint8_t TX_Sensor_data[TX_Sensor_Packet_length] = {0x76, 0x30, };
uint8_t TX_Sensor_flag = 1;

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
  
  pinMode(pin_DUST_LED, OUTPUT);
  pinMode(pin_DUST_OUT, INPUT);
  digitalWrite(pin_DUST_LED, HIGH);

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
  uint16_t Sensor_Data;
  uint8_t i;
  Usb.Task(); // The SPP data is actually not send until this is called, one could call SerialBT.send() directly as well
  if(SerialBT.connected != 0)
  {
    if(TX_Sensor_flag == 1)
    {
      Sensor_Data = Dust_tr(Dust_Read()); // (ADC data -> [ug/m3])

      TX_Sensor_data[10] = Sensor_Data >> 8;
      TX_Sensor_data[11] = Sensor_Data & 0xFF;

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
  return Sensor_data;
}

// ADC data -> [ug/m3]
float Dust_tr(uint16_t ADC_data)
{
  float Dust = ADC_data;
  Dust = Dust*5.0/1023.0/5.95 - 0.1;

  return Dust*1000;
}

