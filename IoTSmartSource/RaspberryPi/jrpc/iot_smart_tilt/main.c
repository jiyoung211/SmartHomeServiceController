#include "bt.h"
#include "main.h"

int main(void){
	int i,bt_sockt,tilt_val;
	bt_sockt = bt_init("00:15:83:E7:2B:B6"); //bluetooth initialize
	bt_config(bt_sockt);  //bluetooth configuration
	jrpc_init();
	while(1){
		tilt_val = event_sensor_read(TILT_C);	//bluetooth parse data read 
		event_send(TILT_C,tilt_val);
		printf("\n\n\n tilt = %d\n\n\n",tilt_val);
		sleep(1); // sleep is 1s 
	}
	jrpc_exit();
	bt_release(bt_sockt);
	return 0;
}

