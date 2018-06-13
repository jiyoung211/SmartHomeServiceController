#ifndef __MAIN_H__
#define __MAIN_H__

//Sensor check 
// 1 = Enable 0 = Disable
// ex) PULSE_C 1 = PUSLE Sensor is Enable
// ex) PULSE_C 0 = PUSLE Sensor is Disable
#define PULSE_C 				1
#define TEMPERATURE_C 		2
#define HUMIDITY_C 			3
#define PSD_C 					4
#define IR_DA_C 				5
#define CDS_C 					6
#define GAS_C 					7
#define DUST_C 				8
#define RAIN_C 				9
#define SOILTEMPERATUR_C 	10
#define VR_C 					11
#define GYROSCOPE_X_C 		12
#define GYROSCOPE_Y_C 		13
#define GYROSCOPE_Z_C 		14

//Event check 
#define BUTTON1_C 			15
#define BUTTON2_C 			16
#define SOUND_C 				17
#define TILT_C 				18
#define PIR_C 					19

//Act check 
#define LED1_C 				20
#define LED2_C 				21
#define DC_MOTOR_C 			22
#define STEP_MOTOR_C 		23
#define SERVO_MOTOR_C 		24
#define FND_C 					25
#define LAMP_C			26

//LED Actuator types
#define LED_OFF				0x01
#define LED1_ON				0x02
#define LED2_ON				0x03	
#define LED_ALL				0x04


//Step motor Actuator types
#define STEP_MOTOR_STOP		0x05
#define STEP_MOTOR_FORWARD	0x06
#define STEP_MOTOR_BACKWARD 0x07


#endif /* __MAIN_H__ */
