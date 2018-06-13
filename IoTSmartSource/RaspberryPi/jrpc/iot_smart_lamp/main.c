#include "main.h"

int main(void){
	int i,actFlg,LAMP_Control;
	unsigned char control_data[100];
	unsigned char ble_mac[20] = "F3:68:33:81:60:BA";
	jrpc_init();
	while(1){
		actFlg = act_flag();
		if(actFlg == 1){
			LAMP_Control = act_recv(LAMP_C);
			if(LAMP_Control == 1)
				sprintf(control_data,"sudo gatttool -t random -b %s --char-write-req --handle=0x0011 --value=4C4F4E",ble_mac);
			else if(LAMP_Control == 0)
				sprintf(control_data,"sudo gatttool -t random -b %s --char-write-req --handle=0x0011 --value=4C4F4646",ble_mac);
			system(control_data);
		}
		sleep(1); // sleep is 1s 
	}
	jrpc_exit();
	return 0;
}

