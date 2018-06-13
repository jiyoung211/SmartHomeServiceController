#include "bt.h"
#include "main.h"

int main(void){
	int i,bt_sockt,x_val,y_val,z_val;
	bt_sockt = bt_init("00:15:83:E7:2B:B6"); //bluetooth initialize
	bt_config(bt_sockt);  //bluetooth configuration
	jrpc_init();
	while(1){
		x_val = series_sensor_read(GYROSCOPE_X_C);
		y_val = series_sensor_read(GYROSCOPE_Y_C);
		z_val = series_sensor_read(GYROSCOPE_Z_C);
		sensor_send(GYROSCOPE_X_C,x_val);
		sensor_send(GYROSCOPE_Y_C,y_val);
		sensor_send(GYROSCOPE_Z_C,z_val);
		sleep(60); // sleep is 1m 
	}
	bt_release(bt_sockt);
	return 0;
}

