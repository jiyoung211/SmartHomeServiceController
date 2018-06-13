#include <RFduinoBLE.h>

#define Group_ID  10

int pin_RELAY0_CH = 4;

char Device_Name[] = "DoorLock--";
char Recv_Data[5];
int flag = 0;

void setup() {
  Serial.begin(9600);

  pinMode(pin_RELAY0_CH, OUTPUT);
  digitalWrite(pin_RELAY0_CH, LOW);
  
  Device_Name[8] = Group_ID/10 + '0';
  Device_Name[9] = Group_ID%10 + '0';

  RFduinoBLE.deviceName = Device_Name;
  RFduinoBLE.advertisementData = "D--";
  RFduinoBLE.begin();
}

void loop(){
  // put your main code here, to run repeatedly:
  if(flag)
  {
    digitalWrite(pin_RELAY0_CH, HIGH);
    RFduinoBLE.send(Recv_Data, 5);
    delay(100);
    digitalWrite(pin_RELAY0_CH, LOW);
    Serial.println("Door Control");
    flag = 0;
  }
}

void RFduinoBLE_onReceive(char *data, int len)
{
  if(data[0]=='D'){
    if((data[1] == 'O') && (data[2] == 'P') && (data[3] == 'E') && (data[4] == 'N'))
    {
      for(int i=0; i<5; i++)
      {
        Recv_Data[i] = data[i];
      }
      flag = 1;
    }
  }
}
