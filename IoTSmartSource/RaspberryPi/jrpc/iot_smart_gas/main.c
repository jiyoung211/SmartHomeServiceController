#include "bt.h"
#include "main.h"

int main(void){
	int i,bt_sockt,gas_val;
	bt_sockt = bt_init("00:15:83:E7:2B:B6"); //bluetooth initialize
	bt_config(bt_sockt);  //bluetooth configuration
	jrpc_init();
	while(1){
		gas_val = series_sensor_read(GAS_C);  //bluetooth parse data read 
		sensor_send(GAS_C,gas_val);
		sleep(60); // sleep is 1m
	}
	jrpc_exit();
	bt_release(bt_sockt);
	return 0;
}

