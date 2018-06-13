#include "bt.h"
#include "main.h"

int main(void){
	int i,bt_sockt,pir_val;
	bt_sockt = bt_init("00:15:83:E7:2B:B6"); //bluetooth initialize
	bt_config(bt_sockt);  //bluetooth configuration
	jrpc_init();
	while(1){
		pir_val = event_sensor_read(PIR_C);	//bluetooth parse data read 
		event_send(PIR_C,pir_val);
		sleep(1); // sleep is 1s 
	}
	jrpc_exit();
	bt_release(bt_sockt);
	return 0;
}

