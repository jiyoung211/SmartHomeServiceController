#include "bt.h"
#include "main.h"

int main(void){
	int i,bt_sockt,temp_val,humi_val,cds_val;
	bt_sockt = bt_init("00:15:83:E7:2B:B6"); //bluetooth initialize
	bt_config(bt_sockt);  //bluetooth configuration
	jrpc_init();
	while(1){
		temp_val = series_sensor_read(TEMPERATURE_C);  //bluetooth parse data read 
		humi_val = series_sensor_read(HUMIDITY_C);  //bluetooth parse data read 
		cds_val = series_sensor_read(CDS_C);	//bluetooth parse data read 
		sensor_send(TEMPERATURE_C,temp_val);
		sensor_send(HUMIDITY_C,humi_val);
		sensor_send(CDS_C,cds_val);
		sleep(60); // sleep is 1m 
	}
	jrpc_exit();
	bt_release(bt_sockt);
	return 0;
}

