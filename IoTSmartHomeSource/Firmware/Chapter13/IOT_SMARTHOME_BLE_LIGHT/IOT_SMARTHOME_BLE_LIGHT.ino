#include <RFduinoBLE.h>

#define Group_ID  13

int pin_RELAY0_CH = 4;
int pin_RELAY0_DETECT = 5;

char Device_Name[] = "LIGHT--";
char Recv_Data[4];
int Light_state = 1;  // light off

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(pin_RELAY0_CH, OUTPUT);
  pinMode(pin_RELAY0_DETECT, INPUT);
  digitalWrite(pin_RELAY0_CH, LOW);
  delay(100);

  Device_Name[5] = Group_ID/10 + '0';
  Device_Name[6] = Group_ID%10 + '0';
  
  RFduinoBLE.deviceName = Device_Name;
  RFduinoBLE.advertisementData = "L---";
  RFduinoBLE.begin();
}

void loop(){
  // put your main code here, to run repeatedly:
  int Light_detect = digitalRead(pin_RELAY0_DETECT);
  if(Light_state != Light_detect)
  {
    Light_state = Light_detect;
    Serial.print("LIGHT : ");
    if(Light_state)
    {
      Serial.println("OFF");
    }
    else
    {
      Serial.println("ON");
    }    
  }
}

void RFduinoBLE_onReceive(char *data, int len)
{
  if(data[0]=='L'){
    for(int i=0; i<len; i++)
    {
      Recv_Data[i] = data[i];
    }
    if((Recv_Data[1] == 'O') && (Recv_Data[2] == 'N'))
    {
      digitalWrite(pin_RELAY0_CH, HIGH);
      RFduinoBLE.send(Recv_Data, 3);
    }
    else if((Recv_Data[1] == 'O') && (Recv_Data[2] == 'F') && (Recv_Data[3] == 'F'))
    {
      digitalWrite(pin_RELAY0_CH, LOW);
      RFduinoBLE.send(Recv_Data, 4);
    }
  }
}
