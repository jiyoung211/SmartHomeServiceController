#include "bt.h"
#include "main.h"

int main(void){
	int i,bt_sockt,sw1_val,sw2_val;
	bt_sockt = bt_init("00:15:83:E7:2B:B6"); //bluetooth initialize
	bt_config(bt_sockt);  //bluetooth configuration
	for(i=0;i<10;i++){
		sw1_val = event_sensor_read(BUTTON1_C);  //bluetooth pars data read 
		sw2_val = event_sensor_read(BUTTON2_C);  //bluetooth pars data read 
		
		if(sw1_val && sw2_val){
			set_led(LED_ALL);
			printf("LED 1, LED 2 ON\n");
		}else if(sw1_val){
			set_led(LED1_ON);
			printf("LED 1 ON !\n");
		}else if(sw2_val){
			set_led(LED2_ON);
			printf("LED 2 ON !\n");
		}else{
			set_led(LED_OFF);
			printf("LED 1, LED 2 OFF\n");
		}
		
		sleep(3);
	}
	bt_release(bt_sockt);
	return 0;
}
