#include <SPP.h>
#include <SPI.h>
#include <MsTimer2.h>

#define Group_ID  10

#define TX_Sensor_Packet_length 23

int pin_GAS = A3;

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
  
  pinMode(pin_GAS, INPUT);

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
  uint16_t PPM;
  uint8_t i;
  Usb.Task(); // The SPP data is actually not send until this is called, one could call SerialBT.send() directly as well
  if(SerialBT.connected != 0)
  {
    if(TX_Sensor_flag == 1)
    {
      PPM = GAS_tr(analogRead(pin_GAS)/0.6);  // 3V -> 5V

      TX_Sensor_data[12] = PPM >> 8;
      TX_Sensor_data[13] = PPM & 0xFF;

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

uint16_t GAS_tr(uint16_t adc_data)
{
  float ratio; 
  uint16_t ppm;
  ratio = adc_data;
  ratio = ratio/(1023-adc_data);  // Vo/Vs = adc_data/(1023-adc_data)
  if(ratio > 6.7)
  {
    ratio = 6.7;
  }
  ppm = pow(2.71828182,0.7201*ratio)*119.37;
  if(ppm > 10000)
  {
    ppm = 10000;
  }
  return ppm;
}

