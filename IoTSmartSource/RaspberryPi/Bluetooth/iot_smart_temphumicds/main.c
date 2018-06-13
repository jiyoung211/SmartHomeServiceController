#include "bt.h"
#include "main.h"

int main(void){
	int i,bt_sockt,temp_val,humi_val,cds_val;
	bt_sockt = bt_init("00:15:83:E7:2B:B6"); //bluetooth initialize
	bt_config(bt_sockt);  //bluetooth configuration
	for(i=0;i<10;i++){
		temp_val = series_sensor_read(TEMPERATURE_C);  //bluetooth pars data read 
		humi_val = series_sensor_read(HUMIDITY_C);  //bluetooth pars data read 
		cds_val = series_sensor_read(CDS_C);	//bluetooth pars data read 
		printf("temp -> %d , humi -> %d , CDS -> %d\n",temp_val,humi_val,cds_val);
		sleep(10); // sleep is 10s 
	}
	bt_release(bt_sockt);
	return 0;
}
