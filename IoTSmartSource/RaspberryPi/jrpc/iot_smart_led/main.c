#include "bt.h"
#include "main.h"

int main(void){
	int i,bt_sockt,actFlg, LED1_Control,LED2_Control;
	bt_sockt = bt_init("00:15:83:E7:2B:B6"); //bluetooth initialize
	bt_config(bt_sockt);  //bluetooth configuration
	jrpc_init();
	while(1){
		actFlg = act_flag();
		if(actFlg == 1){
			LED1_Control = act_recv(LED1_C);
			LED2_Control = act_recv(LED2_C);
			if(LED1_Control == 3 && LED2_Control == 3)
				set_led(LED_ALL);
			else if(LED1_Control == 1)
				set_led(LED1_ON);
         else if(LED2_Control == 2)
            set_led(LED2_ON);
         else 
				set_led(LED_OFF);
			
		}
		sleep(1); // sleep is 1s 
	}
	jrpc_exit();
	bt_release(bt_sockt);
	return 0;
}

