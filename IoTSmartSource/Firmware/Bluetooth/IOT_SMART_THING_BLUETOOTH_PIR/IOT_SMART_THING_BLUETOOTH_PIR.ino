#include <SPP.h>
#include <SPI.h>

#define Group_ID  10

#define TX_Event_Packet_length 7

int pin_PIR = 19;

uint8_t TX_Event_data[TX_Event_Packet_length] = {0x76, 0x31, };
uint8_t TX_Event_flag = 0, PIR_flag = 0;
uint32_t Time_check_PIR = 0;
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

  for(i=2; i<(TX_Event_Packet_length-1); i++)
  {
    TX_Event_data[i] = 0xFF;
  }
  TX_Event_data[TX_Event_Packet_length-1] = 0x3E;

  pinMode(pin_PIR, INPUT);

  attachInterrupt(digitalPinToInterrupt(pin_PIR), PIR_ISR, RISING);

  Bluetooth_Name[7] = Group_ID/10 + '0';
  Bluetooth_Name[8] = Group_ID%10 + '0';

  //USB init
  if (Usb.Init() == -1) {
    Serial.print(F("\r\nOSC did not start"));
    while(1); //halt
  }
  Serial.print(F("\r\nSPP Bluetooth Library Started"));
}

void loop() {
  // put your main code here, to run repeatedly:
  uint8_t i;
  Usb.Task(); // The SPP data is actually not send until this is called, one could call SerialBT.send() directly as well
  if(SerialBT.connected != 0)
  {
    if(TX_Event_flag == 1)
    {
      SerialBT.write(TX_Event_data, TX_Event_Packet_length);
      
      Serial.println("\n\rTX Event Packet :");
      for(i=0; i<TX_Event_Packet_length; i++)
      {
        Serial.print(TX_Event_data[i], HEX);
        Serial.write(' ');
      }
      
      TX_Event_flag = 0;
    }
    if((PIR_flag == 1) && (Time_check_PIR < millis()))
    {
      PIR_flag = 0;
      TX_Event_data[5] = 0;
      TX_Event_flag = 1;
    }
  }
}

void PIR_ISR(void)
{
  if(PIR_flag != 1)
  {
    PIR_flag = 1;
    TX_Event_data[5] = 1;
    TX_Event_flag = 1;
  }
  Time_check_PIR = millis()+1000;
}


