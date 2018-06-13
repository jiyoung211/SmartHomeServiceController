#include <SPP.h>
#include <SPI.h>

#define Group_ID  10

#define RX_Packet_length 9

int pin_DC_P = 9;
int pin_DC_N = 10;
int pin_DC_EN = 11;

uint8_t RX_data[RX_Packet_length];
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

  pinMode(pin_DC_P, OUTPUT); 
  pinMode(pin_DC_N, OUTPUT); 
  pinMode(pin_DC_EN, OUTPUT); 
  digitalWrite(pin_DC_EN, LOW);
  digitalWrite(pin_DC_P, HIGH);
  digitalWrite(pin_DC_N, LOW);

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
  static uint8_t rx_count = 0;
  uint8_t i;
  char da = 0;
  Usb.Task(); // The SPP data is actually not send until this is called, one could call SerialBT.send() directly as well
  if(SerialBT.connected != 0)
  {
    // receive data check
    if(SerialBT.available() > 0)
    {
      da = SerialBT.read();

      // receive data check
      if((rx_count == 0) && (da == 0x76))
      {
        RX_data[rx_count] = da;
        rx_count++;
      }
      else if(rx_count == 1)
      {
        if(da == 0x32)
        {
          RX_data[rx_count] = da;
          rx_count++;
        }
        else
        {
          rx_count = 0;
        }
      }
      else if(rx_count > 1)
      {
        RX_data[rx_count] = da;
        rx_count++;
        // Complete data received
        if(rx_count == RX_Packet_length)
        {
          rx_count = 0;
          // checksum
          if(RX_data[RX_Packet_length-1] == 0x3E)
          {
            Serial.println("\n\r RX_Data ");
            for(i=0; i<RX_Packet_length; i++)
            {
              Serial.print(RX_data[i], HEX);
              Serial.write(' ');
            }
            if(RX_data[3] != 0)
            {
              analogWrite(pin_DC_EN, RX_data[3] * 25 + 80);
            }
            else
            {
              analogWrite(pin_DC_EN, 0);
            }
          }
        }
      }
    }
  }
}

