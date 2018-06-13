#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <pthread.h>
#include <wiringPi.h>
#include <wiringSerial.h>

#define BUFSIZE 100
#define GROUP_ID 10
#define FALSE 0
#define TRUE 1
#define BUFFER_MAX 64

#define SERIAL_DEV_NAME "/dev/ttyUSB0"
#define SERIAL_SPEED 57600

#define MODULE_ID          0x5E
#define CMD_GET_SENSOR     0x05
#define CMD_FAN_ON         0x0E
#define CMD_FAN_OFF     0x0D
#define FAN_STATE_ON       0x11
#define FAN_STATE_OFF      0x12

#define STATUS_FAN_UNKNOWN    0
#define STATUS_FAN_ON      1
#define STATUS_FAN_OFF     2

typedef struct _tx_packet_header
{
   unsigned char frame_type;
   unsigned char seq;
   unsigned char frame_dispatch;
   unsigned short dest_addr;
   unsigned short src_addr;
   unsigned char payload_length;
   unsigned char group_id;
   unsigned char type;
}__attribute__((packed)) tx_packet_header_t;

typedef struct _rx_packet_header
{
   unsigned char frame_type;
   unsigned char frame_dispatch;
   unsigned short dest_addr;
   unsigned short src_addr;
   unsigned char payload_length;
   unsigned char group_id;
   unsigned char type;
}__attribute__((packed)) rx_packet_header_t;

typedef struct _user_data
{
   unsigned char module_id;
   unsigned char cmd;
   unsigned char subcmd;
   unsigned char data_length;
}__attribute__((packed)) user_data_t;

typedef struct _ctrl_packet
{
   tx_packet_header_t packet_header;
   user_data_t user_data;
   unsigned short crc;
}__attribute__((packed)) ctrl_packet_t;

typedef struct _ack_packet
{
   unsigned char frame_type;
   unsigned char frame_dispatch;
   unsigned short crc;
}__attribute__((packed)) ack_packet_t;

typedef struct _sensor_data
{
   unsigned short cds;
   unsigned short temp;
   unsigned short humi;
   unsigned short light;
   unsigned short bat;
}__attribute__((packed)) sensor_data_t;

typedef struct _shared_context
{
   float temp;
   float humi;
   unsigned fan_status;
}__attribute__((packed)) shared_context_t;

unsigned char exit_flag = FALSE;
unsigned char ack_recved = FALSE;
shared_context_t shared_context = {0,};
int fd = -1;

// crc check function
unsigned short crcByte(unsigned char *data, unsigned char Length)
{
   unsigned short crc = 0, i;
   for (i = 0; i < Length; i++)
   {
      crc = (unsigned char)(crc >> 8) | (crc << 8);
      crc ^= data[i];
      crc ^= (unsigned char)(crc & 0xff) >> 4;
      crc ^= crc << 12;
      crc ^= (crc & 0xff) << 5;
   }
   return crc;
}

// on packet receive
void recv_packet_processing(unsigned char* packet, int size)
{
   unsigned char frame_type = 0;
   unsigned short packet_crc = 0, calc_crc = 0;
   rx_packet_header_t* ppacketheader = NULL;
   user_data_t* puserdata = NULL;
   sensor_data_t* psensor = NULL;
   unsigned char ext_data = 0;
   float temp,humi;

   packet_crc = *(unsigned short*)(packet+size-2); //2 => .... + crc(2byte)
   calc_crc = crcByte(packet, size-2);

   if(packet_crc == calc_crc)
   {
      frame_type = ((ack_packet_t*)packet)->frame_type;

      if(frame_type == 0x43)
      {
         //ack recevied
         ack_recved = TRUE;
      }
      else if(frame_type == 0x45)
      {
         ppacketheader = (rx_packet_header_t*)packet;
         puserdata =  (user_data_t*)(packet + sizeof(rx_packet_header_t));

         if(puserdata->module_id == MODULE_ID)
         {
            piLock(0);
            if(puserdata->cmd == 0x01 && puserdata->subcmd == 0x05 && puserdata->data_length == 0x0a)
            {
               psensor = (sensor_data_t*)(packet + sizeof(rx_packet_header_t) + sizeof(user_data_t));
               temp = psensor->temp;
                     temp = temp * 0.01 - 40;
                     humi = psensor->humi;
                     humi = temp * (0.01 + 0.00008 * humi) - 4.0 + 0.0405 * humi - 0.0000028 * humi * humi;


               shared_context.humi = humi;
               shared_context.temp = temp;

            }
            else if(puserdata->cmd == 0x03)
            {
               if(puserdata->subcmd == FAN_STATE_ON)
                  shared_context.fan_status = STATUS_FAN_ON;
               else if(puserdata->subcmd == FAN_STATE_OFF)
                  shared_context.fan_status = STATUS_FAN_OFF;
            }

            piUnlock(0);
         }
      }
   }
}

// recv thread function
void* recv_thread(void *data)
{
   unsigned char prev_data = 0x7e;
   unsigned char read_data = 0;
   unsigned char data_idx = 0;
   unsigned char i = 0;

   unsigned char encode_detect = FALSE;
   unsigned char packet_valid = FALSE;

   unsigned char rx_data[BUFFER_MAX] = { 0, };
   unsigned char buffer[BUFFER_MAX] = { 0, };

   int readcount = 0;

   while (exit_flag == FALSE)
   {
      if ((readcount = serialDataAvail(fd)) > 0)
      {
         for (i = 0; i < readcount && i < BUFFER_MAX; i++)
            buffer[i] = serialGetchar(fd);
      }
      else{
         continue;
      }
      for (i = 0; i < readcount; i++)
      {
         read_data = buffer[i];

         if (prev_data == 0x7e && read_data == 0x7e)
         {
            //packet start detected
            memset(rx_data, sizeof(rx_data), 0);
            packet_valid = TRUE;
            data_idx = 0;
         }
         else if (prev_data != 0x7e && read_data == 0x7e)
         {
            //packet end detected
            if (packet_valid == TRUE)
            {
               if (data_idx < sizeof(rx_data))
               {
                  recv_packet_processing(rx_data, data_idx);
               }
               packet_valid = FALSE;
            }
         }
         else if (read_data == 0x7d && packet_valid == TRUE)
         {
            encode_detect = TRUE;
         }
         else if (packet_valid == TRUE)
         {
            if(encode_detect == TRUE)
            {
               read_data ^= 0x20;
               encode_detect = FALSE;
            }

            if (data_idx < sizeof(rx_data))
            {
               rx_data[data_idx++] = read_data;
            }
            else
            {
               packet_valid = FALSE;
            }
         }

         prev_data = read_data;
      }
   }

   return 0;
}

unsigned char send_command(unsigned char cmd, unsigned char subcmd)
{
   int idx = 0, tx_idx = 0;
   int writeRsult, readRsult;
   unsigned char tx_data[BUFFER_MAX] = {0,};
   ctrl_packet_t packet = { 0, };
   unsigned char* ptemp = NULL;

   packet.packet_header.frame_type = 0x44;
   packet.packet_header.seq = 00;
   packet.packet_header.frame_dispatch = 0x00;
   packet.packet_header.dest_addr = 0xffff;
   packet.packet_header.src_addr = 0xffff;
   packet.packet_header.payload_length = sizeof(user_data_t);
   packet.packet_header.group_id = 00;
   packet.packet_header.type = 0x40;

   packet.user_data.module_id = MODULE_ID;
   packet.user_data.cmd = cmd;
   packet.user_data.subcmd = subcmd;
   packet.user_data.data_length = 0;

   packet.crc = crcByte((unsigned char*)&packet.packet_header, sizeof(tx_packet_header_t) + sizeof(user_data_t));

   tx_data[tx_idx++] = 0x7e;  //start
   ptemp = (unsigned char*)&packet;
   for(idx = 0; idx < sizeof(packet) && tx_idx < BUFFER_MAX-1; idx++)
   {
      if(ptemp[idx] == 0x7e || ptemp[idx] == 0x7d)
      {
         tx_data[tx_idx++] = 0x7d;
         tx_data[tx_idx++] = ptemp[idx]^0x20;
      }
      else
      {
         tx_data[tx_idx++] = ptemp[idx];
      }
   }
   tx_data[tx_idx++]  = 0x7e; //end
   for (idx = 0; idx < tx_idx; idx++)
   {
      serialPutchar(fd, tx_data[idx]);
      delayMicroseconds(1);
   }

   return TRUE;
}

void * clnt_connection(void *arg);
void send_message(char * message, int len);
void error_handling(char *message);
void *zig_thread(void *data);
void *blue_thread(void *data);
void status_message(int i);
int clnt_number=0;
int clnt_socks;
pthread_mutex_t mutx;
pthread_t pth_zig,pth_blue;
int  zig_thid,blue_thid;
int Auto_flg =0, Recv_flg=0;
float cur_temp=0,cur_humi=0;
unsigned char fan_status = STATUS_FAN_UNKNOWN;
unsigned char fanOnTemp = 26;
unsigned char fanOffTemp = 25;
char str2[50];

int main(void)
{
   system("sudo hciconfig hci0 down");
   system("sudo hciconfig hci0 up");
   system("sudo hciconfig hci0 piscan");
   system("sudo sdptool add sp");
   if (wiringPiSetup() == -1)
   {
      fprintf(stderr, "Unable to start wiringPi: %s\n", strerror(errno));
      exit(1);
   }
   if ((fd = serialOpen(SERIAL_DEV_NAME, SERIAL_SPEED)) < 0)
   {
      fprintf(stderr, "Unable to open serial device: %s\n", strerror(errno));
      exit(1);
   }
   if (piThreadCreate(recv_thread) < 0)
   {
      fprintf(stdout, "Unable to create thread: %s\n", strerror(errno));
      exit(1);
   }
   blue_thid = pthread_create(&pth_blue, NULL, blue_thread,0);
   zig_thid = pthread_create(&pth_zig,NULL,zig_thread,0);
   while(1){
   }

   return 0;
}
void *blue_thread(void *data){

   int   fd,client,option=1;
   struct sockaddr_rc loc_addr = { 0 }, rem_addr = { 0 };
   pthread_t thread;
   int opt = sizeof(rem_addr);
   char buf[50]={0};
   if(pthread_mutex_init(&mutx, NULL))
      error_handling("mutex init error");

   fd = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
   if(fd == -1)
      error_handling("socket() error");

   loc_addr.rc_family = AF_BLUETOOTH;
  loc_addr.rc_bdaddr = *BDADDR_ANY;
  loc_addr.rc_channel = (uint8_t) 1;

   if(bind(fd, (struct sockaddr *)&loc_addr, sizeof(loc_addr))==-1)
      error_handling("bind() error");

   if(listen(fd, 5)==-1)
      error_handling("listen() error");
   while(1){
      client = accept(fd, (struct sockaddr *)&rem_addr, &opt);

      pthread_mutex_lock(&mutx);
      clnt_socks=client;
      pthread_mutex_unlock(&mutx);

      pthread_create(&thread, NULL, clnt_connection, (void*)client);
      ba2str( &rem_addr.rc_bdaddr, buf );
   }
}
void *zig_thread(void *data){
   do
   {
      send_command(0x02, CMD_FAN_ON);
      send_command(0x02, CMD_FAN_OFF);
      delay(100);
      while (TRUE)
      {
         send_command(0x01, CMD_GET_SENSOR);
         delay(100); //wait 100ms

         piLock(0);
         cur_temp = shared_context.temp;
         cur_humi = shared_context.humi;
         fan_status = shared_context.fan_status;
         piUnlock(0);

         printf("temperature : %0.2f , humidity :  %0.2f, ", cur_temp, cur_humi);
         if(fan_status == STATUS_FAN_UNKNOWN)
            printf("FAN state : UNKNOWN\n");
         else if(fan_status == STATUS_FAN_ON)
            printf("FAN state : ON\n");
         else if(fan_status == STATUS_FAN_OFF)
            printf("FAN state : OFF\n");
         if(Auto_flg == 1){
            if(cur_temp >= fanOnTemp)
               send_command(0x02, CMD_FAN_ON);
            else if(cur_temp <= fanOffTemp)
               send_command(0x02, CMD_FAN_OFF);
         }
         if(Recv_flg == 1)
            status_message(1);


         delay(1000);
      }

   } while (FALSE);

   if (fd >= 0)
      serialClose(fd);
}
void * clnt_connection(void *arg)
{
   int clnt_sock= (int)arg;
   int str_len=0;
   unsigned char message[BUFSIZE];
   int i;

   while( (str_len=read(clnt_sock, message, sizeof(message))) != 0)
      send_message(message, str_len);
   pthread_mutex_lock(&mutx);
   for(i=0; i<clnt_number; i++){
      if(clnt_sock == clnt_socks){
         break;
      }
   }
   clnt_number--;
   pthread_mutex_unlock(&mutx);

   close(clnt_sock);
   return 0;
}

void send_message(char * message, int len)
{
   int i, groupid;
   char *str = NULL, *temp1 = NULL,*temp2 = NULL;
   message[len]=0;
   if(!strncmp(message,"groupid",7)){
      str = strtok(message,",");
      temp1 = strtok(NULL,":");
      groupid = atoi(temp1);
      if(groupid ==  GROUP_ID){
         printf("Group id OK\n");
      }else{
         error_handling("Group id No Equals\n");
      }
   }else if(!strcmp(message,"0\n")){
      send_command(0x02,CMD_FAN_ON);
      fan_status = 1;
   }else if(!strcmp(message,"1\n")){
      send_command(0x02,CMD_FAN_OFF);
      fan_status = 2;
   }else if(!strncmp(message,"2",1)){
      str = strtok(message,",");
      temp1 = strtok(NULL,":");
      temp2 = strtok(NULL,":");
      Auto_flg=1;
      fanOnTemp = atoi(temp1);
      fanOffTemp  = atoi(temp2);
   }else if(!strcmp(message,"3\n")){
      send_command(0x02,CMD_FAN_OFF);
      Auto_flg=0;
   }else if(!strcmp(message,"4\n")){
      Recv_flg=1;
   }else if(!strcmp(message,"5\n")){
      Recv_flg=2;
      status_message(2);
   }
   bzero(message, sizeof(message));
}
void status_message(int i){
   if(i == 1)
      sprintf(str2,"Fan:%d Temp:%0.2f \nHumi:%0.2f",fan_status,cur_temp,cur_humi);
   else
      sprintf(str2,"Fan:0 Temp:0 \nHumi:0");
   pthread_mutex_lock(&mutx);
   write(clnt_socks,str2,sizeof(str2));
   pthread_mutex_unlock(&mutx);


}

void error_handling(char *message)
{
   fputs(message, stderr);
   fputc('\n', stderr);
   exit(1);
}

