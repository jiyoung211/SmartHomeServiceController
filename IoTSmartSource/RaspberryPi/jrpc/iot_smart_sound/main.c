#include "bt.h"
#include "main.h"

int main(void){
	int i,bt_sockt,sound_val;
	bt_sockt = bt_init("00:15:83:E7:2B:B6"); //bluetooth initialize
	bt_config(bt_sockt);  //bluetooth configuration
	jrpc_init();
	while(1){
		sound_val = event_sensor_read(SOUND_C);	//bluetooth parse data read 
		event_send(SOUND_C,sound_val);
		sleep(1); // sleep is 1s 
	}
	jrpc_exit();
	bt_release(bt_sockt);
	return 0;
}

