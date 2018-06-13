#include "bt.h"
#include "main.h"

int main(void){
	int i,bt_sockt,sw1_val, sw2_val;
	bt_sockt = bt_init("00:15:83:E7:2B:B6"); //bluetooth initialize
	bt_config(bt_sockt);  //bluetooth configuration
	jrpc_init();
	while(1){
		sw1_val = event_sensor_read(BUTTON1_C);	//bluetooth parse data read 
		sw2_val = event_sensor_read(BUTTON2_C);	//bluetooth parse data read 
		
		if(sw1_val && sw2_val){
			event_send(BUTTON1_C,sw1_val);
			event_send(BUTTON2_C,sw2_val);
		}else if(sw1_val)
			event_send(BUTTON1_C,sw1_val);
		else if(sw2_val)
			event_send(BUTTON2_C,sw2_val);
		else {
			event_send(BUTTON1_C,0);
			event_send(BUTTON2_C,0);
			
		}	
		sleep(1); // sleep is 1s 
	}
	jrpc_exit();
	bt_release(bt_sockt);
	return 0;
}

