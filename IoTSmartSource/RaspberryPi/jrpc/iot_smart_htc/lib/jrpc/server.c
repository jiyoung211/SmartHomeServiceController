#include <time.h>
#include "jrpc_server.h"
#include "bt.h"

#define PORT 50800  // Server Port
#define DEVICE_ID "b827eb0e7699" //Raspberry PI MAC Address
//Sensors name Setting = DEVICE_ID + "-SensorType-index"

//sensor ID 
#define TEMP0 DEVICE_ID "-temperature-0"
#define DUST0 DEVICE_ID "-dust-0"
#define RAIN0 DEVICE_ID "-rain-0"
#define SOIL0 DEVICE_ID "-soiltemperature-0"
#define GYRO_X0 DEVICE_ID "-gyroscope-x-0"
#define GYRO_Y0 DEVICE_ID "-gyroscope-y-0"
#define GYRO_Z0 DEVICE_ID "-gyroscope-z-0"
#define PULSE0 DEVICE_ID "-pulse-0"
#define CDS0 DEVICE_ID "-light-0"
#define HUMI0 DEVICE_ID "-humidity-0"
#define PSD0 DEVICE_ID "-psd-0"
#define GAS0 DEVICE_ID "-gas-0"
#define VR0 DEVICE_ID "-vr-0"
#define IR0 DEVICE_ID "-ir-0"

//ACT ID
#define LED1 DEVICE_ID "-led-1"
#define LED2 DEVICE_ID "-led-2"
#define DCMOTOR0 DEVICE_ID "-dcmotor-0"
#define STEPMOTOR0 DEVICE_ID "-stepmotor-0"
#define SERVOMOTOR0 DEVICE_ID "-servomotor-0"
#define FND0 DEVICE_ID "-fnd-0"

//Event ID
#define PIR0 DEVICE_ID "-motion-0"
#define BUTTON1 DEVICE_ID "-button-1"
#define BUTTON2 DEVICE_ID "-button-2"
#define SOUND0 DEVICE_ID "-sound-0"
#define TILT0 DEVICE_ID "-tilt-0"

series_sensor_send_data_t series_send_data;
event_sensor_send_data_t event_send_data;
act_recv_data_t act_recv_data;
event_sensor_temp_data_t event_temp_data;
void handle_kill_signal() {
	jrpc_server_stop(my_server);
	signal(SIGINT, SIG_DFL);
	signal(SIGTERM, SIG_DFL);
	signal(SIGHUP, SIG_DFL);
}
// AtThing + Potal Sensor Set
cJSON *discover(jrpc_context_t *ctx, cJSON *params, cJSON *id) {
	cJSON* devices = NULL, *device = NULL, *sensor = NULL, *all=NULL, *sensors = NULL;
  all = cJSON_CreateObject();
  devices = cJSON_CreateArray();
  device = cJSON_CreateObject();  
  cJSON_AddItemToArray(devices, device);

  cJSON_AddItemToObject(all, "jsonrpc",cJSON_CreateString(JRPC_VERSION));
  cJSON_AddItemToObject(device, "deviceAddress", cJSON_CreateString(DEVICE_ID));

   sensors = cJSON_CreateArray();

  //temp0
	  sensor = cJSON_CreateObject();
	  cJSON_AddStringToObject(sensor, "id", TEMP0);
	  cJSON_AddStringToObject(sensor, "type", "temperature");
	  cJSON_AddStringToObject(sensor, "name", "temp0");
	  cJSON_AddItemToArray(sensors, sensor);
  
  //humi0
	  sensor = cJSON_CreateObject();
	  cJSON_AddStringToObject(sensor, "id", HUMI0);
	  cJSON_AddStringToObject(sensor, "type", "humidity");
	  cJSON_AddStringToObject(sensor, "name", "humi0");
	  cJSON_AddItemToArray(sensors, sensor);

  //cds0
	  sensor = cJSON_CreateObject();
	  cJSON_AddStringToObject(sensor, "id", CDS0);
	  cJSON_AddStringToObject(sensor, "type", "light");
	  cJSON_AddStringToObject(sensor, "name", "cds0");
	  cJSON_AddItemToArray(sensors, sensor);
	  
  cJSON_AddItemToObject(device, "sensors", sensors);
	
  cJSON_AddItemToObject(all, "result", devices);
	return all;
}

// 
cJSON *sensor_set(jrpc_context_t *ctx, cJSON *params, cJSON *id) {
  cJSON *result = cJSON_CreateObject(),*result2 = cJSON_CreateObject(),
  //cJSON *result = NULL,*error = NULL,
        *sensorId = NULL,*sensorId2 = NULL;
  char *sensorIdStr = NULL,*sensorIdStr2 = NULL;
 
	cJSON_AddItemToObject(result2, "jsonrpc",cJSON_CreateString(JRPC_VERSION));
    sensorId = cJSON_GetArrayItem(params, 0);
    sensorId2 = cJSON_GetArrayItem(params, 1);
    if (NULL != sensorId) {
      sensorIdStr = sensorId->valuestring;
      sensorIdStr2 = sensorId2->valuestring;
    }
	if (sensorIdStr != NULL) { //very simple simulation
		if(strcmp(sensorIdStr, LED1) == 0) {
			if(!strcmp(sensorIdStr2,"on")){	 
				result = cJSON_CreateString("on");
				cJSON_AddStringToObject(result, "error", "NULL");
				act_recv_data.led1 = 1;
			}else if(!strcmp(sensorIdStr2,"off")){	
				result = cJSON_CreateString("off");
				cJSON_AddStringToObject(result, "error", "NULL");
				act_recv_data.led1 = 0;
			}
		}else if(strcmp(sensorIdStr,LED2) == 0){
			if(!strcmp(sensorIdStr2,"on")){	 
				result = cJSON_CreateString("on");
				cJSON_AddStringToObject(result, "error", "NULL");
				act_recv_data.led2 = 2;
			}else if(!strcmp(sensorIdStr2,"off")){	
				result = cJSON_CreateString("off");
				cJSON_AddStringToObject(result, "error", "NULL");
				act_recv_data.led2 = 0;
			}	
		}else if(strcmp(sensorIdStr,DCMOTOR0)== 0){
			if(!strcmp(sensorIdStr2,"on")){	 
				result = cJSON_CreateString("on");
				cJSON_AddStringToObject(result, "error", "NULL");
				act_recv_data.dc_motor = 2;
			}else if(!strcmp(sensorIdStr2,"off")){	
				result = cJSON_CreateString("off");
				cJSON_AddStringToObject(result, "error", "NULL");
				act_recv_data.dc_motor = 0;
			}	
		}else if(strcmp(sensorIdStr,STEPMOTOR0)== 0){
			if(!strcmp(sensorIdStr2,"on")){	 
				result = cJSON_CreateString("on");
				cJSON_AddStringToObject(result, "error", "NULL");
				act_recv_data.step_motor = 2;
			}else if(!strcmp(sensorIdStr2,"off")){	
				result = cJSON_CreateString("off");
				cJSON_AddStringToObject(result, "error", "NULL");
				act_recv_data.step_motor = 0;
			}	
		}else if(strcmp(sensorIdStr,SERVOMOTOR0)== 0){
			if(!strcmp(sensorIdStr2,"on")){	 
				result = cJSON_CreateString("on");
				cJSON_AddStringToObject(result, "error", "NULL");
				act_recv_data.servo_motor = 2;
			}else if(!strcmp(sensorIdStr2,"off")){	
				result = cJSON_CreateString("off");
				cJSON_AddStringToObject(result, "error", "NULL");
				act_recv_data.servo_motor = 0;
			}	
		}
		act_flg = 1;
	}
	sleep(1);
	act_flg = 0;
	cJSON_AddItemToObject(result2, "result",result);
	return result2;
}

// send Event sensor function
cJSON *sensor_notification(int event) {	
  cJSON *result = cJSON_CreateObject(),*params = cJSON_CreateArray(),*status = cJSON_CreateObject();
  int re = 0;
  char *head = "{\"method\":\"sensor.notification\",\"params\":[\"";
  char *end = "\",{\"value\":";
  char msg[200];
  
  switch(event){
		//button1
		case 1:
			if(BUTTON1_C == 15){
				if(event_send_data.button1 != event_temp_data.button1){	
					if(event_send_data.button1 == 1 || event_send_data.button1 == 3){
						sprintf(msg,"%s%s%s%d}]}",head,BUTTON1,end,1);
						send_notification(gateway_fd,msg);
					}else {
						sprintf(msg,"%s%s%s%d}]}",head,BUTTON1,end,0);
						send_notification(gateway_fd,msg);
					}
						event_temp_data.button1 = event_send_data.button1;
				}
			}
		break;
		//button2
		case 2:
			if(BUTTON2_C == 16){
				if(event_send_data.button2 != event_temp_data.button2){	
					if(event_send_data.button2 == 2 || event_send_data.button2 == 3){
						sprintf(msg,"%s%s%s%d}]}",head,BUTTON2,end,1);
						send_notification(gateway_fd,msg);
					}else {
						sprintf(msg,"%s%s%s%d}]}",head,BUTTON2,end,0);
						send_notification(gateway_fd,msg);
					}
						event_temp_data.button2 = event_send_data.button2;
				}
			}
		break;
		//sound
		case 3:
			if(SOUND_C == 17){
				if(event_send_data.sound != event_temp_data.sound){	
					sprintf(msg,"%s%s%s%d}]}",head,SOUND0,end,event_send_data.sound);
					send_notification(gateway_fd,msg);
					event_temp_data.sound = event_send_data.sound;
				}
			}
		break;
		//tilt
		case 4:
			if(TILT_C == 18){
				if(event_send_data.tilt != event_temp_data.tilt){	
					sprintf(msg,"%s%s%s%d}]}",head,TILT0,end,event_send_data.tilt);
					send_notification(gateway_fd,msg);
					event_temp_data.tilt = event_send_data.tilt;
				}
			}
		break;
		//pir
		case 5:
			if(PIR_C == 19){
				if(event_send_data.pir != event_temp_data.pir){	
					sprintf(msg,"%s%s%s%d}]}",head,PIR0,end,event_send_data.pir);
					send_notification(gateway_fd,msg);
					event_temp_data.pir = event_send_data.pir;
				}
			}
		break;
	}
	sleep(1);
	eventFlg = 0;
	
	return result;
}


cJSON *ping(jrpc_context_t *ctx, cJSON *params, cJSON *id) {
  cJSON *result = cJSON_CreateObject();
  
	cJSON_AddNullToObject(result, "result");
  return result;
}


cJSON *sensor_setNotification(jrpc_context_t *ctx, cJSON *params, cJSON *id) {
	
  cJSON *result = cJSON_CreateObject();
  cJSON_AddStringToObject(result, "result", "success");
  cJSON_AddNullToObject(result, "error");
	return result;
}

//서버로부터 요청을 받아 해당 데이터의 데이터 전송
cJSON *sensor_get(jrpc_context_t *ctx, cJSON *params, cJSON *id) {
  cJSON *result = cJSON_CreateObject(),  *result2 = cJSON_CreateObject(),  
        *sensorId = NULL;
  char *sensorIdStr = NULL;
  
	cJSON_AddItemToObject(result2, "jsonrpc",cJSON_CreateString(JRPC_VERSION));
    sensorId = cJSON_GetArrayItem(params, 0);
	if (NULL != sensorId) 
		sensorIdStr = sensorId->valuestring;
	
	if (sensorIdStr != NULL) { //very simple simulation
		if(strcmp(sensorIdStr, PULSE0) == 0) 
			cJSON_AddNumberToObject(result, "value", series_send_data.pulse); 
		else if(strcmp(sensorIdStr, TEMP0) == 0 )  
			cJSON_AddNumberToObject(result, "value", series_send_data.temp);
		else if(strcmp(sensorIdStr, HUMI0) == 0) 
			cJSON_AddNumberToObject(result, "value", series_send_data.humi);
		else if(strcmp(sensorIdStr, PSD0) == 0) 
			cJSON_AddNumberToObject(result, "value", series_send_data.psd);
		else if(strcmp(sensorIdStr, IR0) == 0) 
			cJSON_AddNumberToObject(result, "value", series_send_data.ir);
		else if(strcmp(sensorIdStr, CDS0) == 0) 
			cJSON_AddNumberToObject(result, "value", series_send_data.cds); 
		else if(strcmp(sensorIdStr, GAS0) == 0) 
			cJSON_AddNumberToObject(result, "value", series_send_data.gas);
		else if(strcmp(sensorIdStr, DUST0) == 0) 
			cJSON_AddNumberToObject(result, "value", series_send_data.dust); 
		else if(strcmp(sensorIdStr, RAIN0) == 0) 
			cJSON_AddNumberToObject(result, "value", series_send_data.rain); 
		else if(strcmp(sensorIdStr, SOIL0) == 0) 
			cJSON_AddNumberToObject(result, "value", series_send_data.soil);
		else if(strcmp(sensorIdStr, VR0) == 0) 
			cJSON_AddNumberToObject(result, "value", series_send_data.vr); 
		else if(strcmp(sensorIdStr, GYRO_X0) == 0) //GYRO X 
			cJSON_AddNumberToObject(result, "value", series_send_data.gyro_x); 
		else if(strcmp(sensorIdStr, GYRO_Y0) == 0) //GYRO Y
			cJSON_AddNumberToObject(result, "value", series_send_data.gyro_y); 
		else if(strcmp(sensorIdStr, GYRO_Z0) == 0) //GYRO Z
			cJSON_AddNumberToObject(result, "value", series_send_data.gyro_z); 
	}
	cJSON_AddItemToObject(result2, "result",result);

	return result2;
}

// Server connection close function
cJSON *exit_server(jrpc_context_t *ctx, cJSON *params, cJSON *id) {
	jrpc_server_stop(my_server);
	return cJSON_CreateString("Bye!");
}

// event sesnsor send thread function
void *notification_thread(void *arg){
	struct tm *t;
	time_t timer;
	while(1){
		timer = time(NULL);
		t = localtime(&timer);
		DBG("%d day, %d h, %d m, %d s\n",t->tm_mday, t->tm_hour+9, t->tm_min, t->tm_sec);
		sleep(60);
				
	}
}

//jrpc thread create function
void *jrpc_thread_create(void *arg){
	struct sigaction action;
	pthread_t pth_noti;
	act_flg = 0;
	is_GW_running = 0;
	eventFlg = 0;
	/*bt_pid=  pthread_create(&pth_processing, NULL, bt_processing, 0);*/
	pthread_create(&pth_noti,0,notification_thread,0);
	pthread_detach(pth_noti);
	
	setenv("JRPC_DEBUG", "1", 1); /* uncomment to active debug */

	my_server = jrpc_server_init(PORT);
	jrpc_server_init_socket(my_server, PORT);
	jrpc_register_procedure(&my_server->procedure_list, discover, "discover", NULL);
	jrpc_register_procedure(&my_server->procedure_list, sensor_get, "sensor.get", NULL);
	jrpc_register_procedure(&my_server->procedure_list, sensor_set, "sensor.set", NULL);
	jrpc_register_procedure(&my_server->procedure_list, ping, "ping", NULL);
	jrpc_register_procedure(&my_server->procedure_list, sensor_setNotification, "sensor.setNotification", NULL);
	jrpc_register_procedure(&my_server->procedure_list, exit_server, "exit", NULL);
	//is_GW_running = my_server->is_running;
	// Add signal handler to terminate server
	action.sa_handler = handle_kill_signal;
	sigemptyset(&action.sa_mask);
	action.sa_flags = 0;
	sigaction(SIGINT, &action, NULL);
	sigaction(SIGTERM, &action, NULL);
	sigaction(SIGHUP, &action, NULL);

	jrpc_server_run(my_server);

	jrpc_server_destroy(my_server);
	if (my_server->debug_level)
		printf("close jrpc-server\n");
	//return 0;
}

// jrpc init function
int jrpc_init() {
	pthread_t pth_jrpc;
	pthread_create(&pth_jrpc,0,jrpc_thread_create,0);
	pthread_detach(pth_jrpc);
	return 0;
}

int jrpc_exit(){
	jrpc_server_stop(my_server);
	
}
//
void sensor_send(int sensor,int value){
	switch(sensor){
		case PULSE_C : 
			series_send_data.pulse = value;
		break;
		case TEMPERATURE_C : 
			series_send_data.temp = value;
		break;
		case HUMIDITY_C : 
			series_send_data.humi = value;
		break;
		case PSD_C : 
			series_send_data.psd = value;
		break;
		case IR_DA_C : 
			series_send_data.ir = value;
		break;
		case CDS_C : 
			series_send_data.cds = value;
		break;
		case GAS_C : 
			series_send_data.gas = value;
		break;
		case DUST_C : 
			series_send_data.dust = value;
		break;
		case RAIN_C : 
			series_send_data.rain = value;
		break;
		case SOILTEMPERATUR_C : 
			series_send_data.soil = value;
		break;
		case VR_C : 
			series_send_data.vr = value;
		break;
		case GYROSCOPE_X_C : 
			series_send_data.gyro_x= value;
		break;
		case GYROSCOPE_Y_C : 
			series_send_data.gyro_y = value;
		break;
		case GYROSCOPE_Z_C : 
			series_send_data.gyro_z = value;
		break;
		
	}
}
void event_send(int event,int value){
	switch(event){
		case BUTTON1_C : 
			event_send_data.button1 = value;
			sensor_notification(1);
		break;
		case BUTTON2_C :
			event_send_data.button2 = value;
			sensor_notification(2);
		break;
		case TILT_C :
			event_send_data.tilt = value;
			sensor_notification(3); 
		break;
		case SOUND_C : 
			event_send_data.sound = value;
			sensor_notification(4);
		break;
		case PIR_C : 
			event_send_data.pir = value;
			sensor_notification(5);
		break;
		
	}
	eventFlg =1;
}

int act_recv(int sensor){
	switch(sensor){
		case LED1_C:
		case LED2_C:
			if(act_recv_data.led1 == 1 && act_recv_data.led2 == 2)
				return 3;
			else if(act_recv_data.led1 == 1)
				return act_recv_data.led1;
			else if(act_recv_data.led2 == 2)
				return act_recv_data.led2;
			else 
				return 0;
		break;
		case DC_MOTOR_C:
			return act_recv_data.dc_motor;
		break;
		case STEP_MOTOR_C:
			return act_recv_data.step_motor;
		break;
		case SERVO_MOTOR_C:
			return act_recv_data.servo_motor;
		break;
		case FND_C:
			return act_recv_data.fnd;
		break;
	}
	
}

int act_flag(){
	return act_flg;
}
