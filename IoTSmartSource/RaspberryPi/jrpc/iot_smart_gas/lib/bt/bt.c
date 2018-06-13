#include "bt.h"    //include header files
#include "main.h"

int sendFlg=0,actBtTemp=0,mConnect=1,eventFlg=0;
pthread_t pth_send,pth_recv;
int send_pth = 0, recv_pth = 0;


//Send Thread function
void * send_message(void *arg){ 
	int sockt = (int)arg;
	int i,res,actCount=0;
	memset(&actuator_packet,0,sizeof(actuator_t));
	actuator_packet.stx = 0x76;
	actuator_packet.cmd = 0x32;
	actuator_packet.etx = 0x3E;
	while(!send_pth) {
		if(sendFlg == 1){
			res = send(sockt, &actuator_packet,sizeof(actuator_t), MSG_NOSIGNAL);
			sendFlg = 0;
			sleep(3);
		}
	}
	close((int)arg);
}


//Led Control function
void set_led(int control){ 
	switch(control){
		case LED_OFF: 
			actuator_packet.led = 0x00;
			break;
		case LED1_ON: 
			actuator_packet.led = 0x01;
			break;
		case LED2_ON: 	
			actuator_packet.led = 0x02;
			break;
		case LED_ALL: 
			actuator_packet.led = 0x03;
			break;
	}
	sendFlg=1;
}

//DC Motor Control function
void set_DC(int level){ 
	switch(level){
		case 0: 
			actuator_packet.dc_motor = 0x00;
			break;
		case 1: 
			actuator_packet.dc_motor = 0x01;
			break;
		case 2: 	
			actuator_packet.dc_motor = 0x02;
			break;
		case 3: 
			actuator_packet.dc_motor = 0x03;
			break;
		case 4: 
			actuator_packet.dc_motor = 0x04;
			break;
		case 5: 
			actuator_packet.dc_motor = 0x05;
			break;						
	}
	sendFlg=1;
}

//Step Motor Control function
void set_step(int direction){ 

	switch(direction){
		case STEP_MOTOR_STOP: 
			actuator_packet.step_motor = 0x00;
			break;
		case STEP_MOTOR_FORWARD: 
			actuator_packet.step_motor = 0x01;
			break;
		case STEP_MOTOR_BACKWARD: 	
			actuator_packet.step_motor = 0x02;
			break;
	}	
	sendFlg=1;
}

//Servo Motor Control function
void set_servo(int angle){ 
	actuator_packet.servo_motor = angle;
	sendFlg=1;
}

//FND Control function
void set_FND(int string){ 
	sendFlg=1;
}

//Return Received  ADK Sensor information 
int  series_sensor_read(int arg){
	int temp=0;
	
	switch(arg){
		case PULSE_C:
			temp=series_sensor_data.pulse;
			break;
		case TEMPERATURE_C:
			temp=series_sensor_data.temp;
			break;
		case HUMIDITY_C:
			temp=series_sensor_data.humi;
			break;
		case PSD_C:
			temp=series_sensor_data.psd;
			break;
		case IR_DA_C:
			temp=series_sensor_data.ir;
			break;
		case CDS_C:
			temp=series_sensor_data.cds;
			break;
		case GAS_C:
			temp=series_sensor_data.gas;
			break;
		case DUST_C:
			temp=series_sensor_data.dust;
			break;
		case RAIN_C:
			temp=series_sensor_data.rain;
			break;
		case SOILTEMPERATUR_C:
			temp=series_sensor_data.soil;
			break;
		case VR_C:
			temp=series_sensor_data.vr;
			break;
		case GYROSCOPE_X_C:
			temp=series_sensor_data.gyro_x;
			break;
		case GYROSCOPE_Y_C:
			temp=series_sensor_data.gyro_y;
			break;
		case GYROSCOPE_Z_C:
			temp=series_sensor_data.gyro_z;
			break;
	}
	return temp;
}

//Return Received  Event Sensor information 
int event_sensor_read(int arg){
	int temp=0;
	
	switch(arg){
		case BUTTON1_C:
			if(event_sensor_packet.button == 1 || event_sensor_packet.button == 3)
				temp = 1;
			else 
				temp = 0;
			break;
		case BUTTON2_C:
			if(event_sensor_packet.button == 2 || event_sensor_packet.button == 3)
				temp = 1;
			else
				temp = 0;
			break;
		case TILT_C:
			temp= event_sensor_packet.tilt;
			break;
		case SOUND_C:
			temp= event_sensor_packet.sound;
			break;
		case PIR_C:
			temp= event_sensor_packet.pir;
			break;
	}
	return temp;
}
//Return Received  ADK Sensor ALL information 
event_sensor_t event_sensor_all_read(void ){
 
	return event_sensor_data;
}

//Return Received  Event Sensor ALL information 
series_sensor_data_t series_sensor_all_read(void ){
 
	return series_sensor_data;
}

//Receive Thread function
void * recv_message(void *arg){ 
	char recv_buf[100];
	int sockt = (int)arg;
	int str_len,i;
	time_t now;

	while(!recv_pth){ 
		time(&now);
		if(mConnect == 0){
			str_len = recv(sockt, recv_buf,sizeof(recv_buf),MSG_NOSIGNAL);
					DBG("\nbluetooth Packet : ");
			for(i=0; i<str_len;i++){
				DBG("%02X, ",recv_buf[i]);
			}
			DBG("\n");
		}
		if(str_len==-1){
			recv_pth=1;
		}else{		
				//printf("Debugg1 -- \n");
			if(str_len == 23){
				//printf("Debugg2 -- \n");
				memcpy(&series_sensor_packet,recv_buf,sizeof(series_sensor_t));
				if(series_sensor_packet.stx == 0x76 && series_sensor_packet.cmd == 0x30 && series_sensor_packet.etx == 0x3E){
					//printf("Debugg3 -- \n");
					series_sensor_data.pulse = (int)series_sensor_packet.pulse;
					series_sensor_data.temp = (int)series_sensor_packet.temp;
					series_sensor_data.humi = (int)series_sensor_packet.humi;
					series_sensor_data.psd = (int)series_sensor_packet.psd;
				    series_sensor_data.ir = (int)series_sensor_packet.ir; 
					series_sensor_data.cds = (int)series_sensor_packet.cds;
					series_sensor_data.rain = (int)series_sensor_packet.rain;
					series_sensor_data.soil = (int)series_sensor_packet.soil;

					series_sensor_data.dust = series_sensor_packet.dust_H & 0xff;
					series_sensor_data.dust = (series_sensor_data.dust << 8 | series_sensor_packet.dust_L& 0xff);

					series_sensor_data.gas = series_sensor_packet.gas_H & 0xff;
					series_sensor_data.gas = (series_sensor_data.gas << 8 | series_sensor_packet.gas_L& 0xff);

					series_sensor_data.vr = series_sensor_packet.vr_H & 0xff;
					series_sensor_data.vr = (series_sensor_data.vr << 8 | series_sensor_packet.vr_L& 0xff);
					
					series_sensor_data.gyro_x = series_sensor_packet.gyro_X_H & 0xff;
					series_sensor_data.gyro_x = (series_sensor_data.gyro_x << 8 | series_sensor_packet.gyro_X_L& 0xff);
					
					series_sensor_data.gyro_y = series_sensor_packet.gyro_Y_H & 0xff;
					series_sensor_data.gyro_y = (series_sensor_data.gyro_y << 8 | series_sensor_packet.gyro_Y_L& 0xff);
					
					series_sensor_data.gyro_z = series_sensor_packet.gyro_Z_H & 0xff;
					series_sensor_data.gyro_z = (series_sensor_data.gyro_z << 8 | series_sensor_packet.gyro_Z_L& 0xff);
				}
				else{
					 printf("The error has been occured,while receiving a sensor message" );
				}
			}
			else if(str_len == 7){
				memcpy(&event_sensor_packet,recv_buf,sizeof(event_sensor_t));
				if(event_sensor_packet.stx == 0x76 && event_sensor_packet.cmd == 0x31 && event_sensor_packet.etx == 0x3E){
					// to do something
					eventFlg = 1;
					event_sensor_data.button = (int)event_sensor_packet.button;
					event_sensor_data.tilt = (int)event_sensor_packet.tilt;
					event_sensor_data.sound = (int)event_sensor_packet.sound;
					event_sensor_data.pir = (int)event_sensor_packet.pir; 
					sleep(1);
				}else{
					 printf("The error has been occured,while receiving a event sensor message" );
				}
			}
		}
		memset(&recv_buf,0,sizeof(recv_buf));
		eventFlg =0;
	}
}

void bt_release(int socket){
	close(socket);
	recv_pth = 1;
	send_pth = 1;
	printf("Bluetooth release\n");
}

int event_flag(){
	return eventFlg;
}
//Bluetooth Initialize function
int bt_init(char *bt_name){
	int sockt;
	memset(&event_sensor_data,0,sizeof(event_sensor_t));
	memset(&series_sensor_data,0,sizeof(series_sensor_t));
	struct sockaddr_rc addr = { 0 };

	system("sudo hciconfig hci0 down");
	system("sudo hciconfig hci0 up");
	system("sudo hciconfig hci0 piscan");
	system("sudo sdptool add sp");
	sleep(1);

	printf("Bluetooth bt_init() start..\n");
	while(1){
		if(mConnect == 1){
			while(mConnect){
				close(sockt);
				sockt=socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
				if(sockt==-1){
					printf("Bluetooth Socket Error()\n");
					exit(1);
				}

				addr.rc_family = AF_BLUETOOTH;
				addr.rc_channel = (uint8_t) 1;
				str2ba( bt_name, &addr.rc_bdaddr );

				if(connect(sockt, (struct sockaddr *)&addr, sizeof(addr))==-1){
					close(sockt);
				}else{
					sleep (1);
					printf("Bluetooth Socket Connect OK\n");
					mConnect=0;
					break;
				}
			}
			break;
		}
	}
	printf("Bluetooth bt_init() end..\n");
	return sockt;
}
//Bluetooth Configure function 
void bt_config(int socket){
	pthread_create(&pth_send, NULL, send_message, (void*)socket);
	pthread_create(&pth_recv, NULL, recv_message, (void*)socket);
	pthread_detach(pth_send);
	pthread_detach(pth_recv);
}
