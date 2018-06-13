#include "zigbee.h"
#include "main.h"

int main(void){
	int actFlg, FAN_Control;	

	jrpc_init();
	while(1){
		actFlg = act_flag();
		if(actFlg == 1){
			FAN_Control = act_recv(FAN_C);
			if(FAN_Control == 1)
				send_command(0x02, CMD_FAN_ON);
			else if(FAN_Control == 0)
				send_command(0x02, CMD_FAN_OFF);
		}
		sleep(1); // sleep is 1s 
	}
	jrpc_exit();
	return 0;
}

		
