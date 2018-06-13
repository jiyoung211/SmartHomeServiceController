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

#define GAS_CB_ID       0x33
#define GAS_D_ID     0x65

#define CMD_GAS_OPEN    0x17
#define CMD_GAS_CUT  0x18
#define CMD_GET_GAS_STATUS 0x07

#define CMD_GET 0x01
#define CMD_SET 0x02

#define STATUS_GAS_UNKNOWN 0
#define STATUS_GAS_OPENED  1
#define STATUS_GAS_CLOSED  2
#define STATUS_GAS_DETECTED   1
#define STATUS_GAS_CLEARED 2

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

unsigned char exit_flag = FALSE;
unsigned char ack_recved = FALSE;
unsigned char gas_cut_status = STATUS_GAS_UNKNOWN;
unsigned char gas_status = STATUS_GAS_UNKNOWN;
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
   unsigned char ext_data = 0;

   packet_crc = *(unsigned short*)(packet + size - 2);   //2 => .... + crc(2byte)
   calc_crc = crcByte(packet, size - 2);

   if (packet_crc == calc_crc)
   {
      frame_type = ((ack_packet_t*)packet)->frame_type;

      if (((ack_packet_t*)packet)->frame_type == 0x43)
      {
         //ack recevied
         ack_recved = TRUE;
      }
      else if(((ack_packet_t*)packet)->frame_type == 0x45)
      {
         ppacketheader = (rx_packet_header_t*)packet;
         puserdata =  (user_data_t*)(packet + sizeof(rx_packet_header_t));

         if(puserdata->module_id == GAS_CB_ID || puserdata->module_id == GAS_D_ID )
         {
            if(puserdata->cmd == 0x01 && puserdata->subcmd == 0x07 && puserdata->data_length == 1)
            {
               printf("");
               ext_data = *(packet + sizeof(rx_packet_header_t) + sizeof(user_data_t));
               if(ext_data == 0)
                  gas_status = STATUS_GAS_DETECTED;
               else //if(ext_data == 1)
                  gas_status = STATUS_GAS_CLEARED;
            }
            if(puserdata->cmd == 0x03)
            {
               if(puserdata->subcmd == 1)
                  gas_status = STATUS_GAS_DETECTED;
               else //if(puserdata->subcmd == 2)
                  gas_status = STATUS_GAS_CLEARED;
            }
         }
      }
   }
}
unsigned char send_command(unsigned char ID, unsigned char cmd, unsigned char subcmd)
{
   int idx = 0, tx_idx = 0;
   int writeRsult, readRsult;
   unsigned char tx_data[BUFFER_MAX] = { 0, };
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

   packet.user_data.module_id = ID;
   packet.user_data.cmd = cmd;
   packet.user_data.subcmd = subcmd;
   packet.user_data.data_length = 0;

   packet.crc = crcByte((unsigned char*)&packet.packet_header, sizeof(tx_packet_header_t) + sizeof(user_data_t));

   tx_data[tx_idx++] = 0x7e;  //start
   ptemp = (unsigned char*)&packet;
   for (idx = 0; idx < sizeof(packet) && tx_idx < BUFFER_MAX - 1; idx++)
   {
      if (ptemp[idx] == 0x7e || ptemp[idx] == 0x7d)
      {
         tx_data[tx_idx++] = 0x7d;
         tx_data[tx_idx++] = ptemp[idx] ^ 0x20;
      }
      else
      {
         tx_data[tx_idx++] = ptemp[idx];
      }
   }
   tx_data[tx_idx++] = 0x7e; //end


   for (idx = 0; idx < tx_idx; idx++)
   {
      serialPutchar(fd, tx_data[idx]);
      delayMicroseconds(1);
   }


   return TRUE;
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

void wait_10sec()
{
   int i = 0;

   printf("wait 10 sec : ");
   for (i = 0; i < 10; i++)
   {
      printf(".");
      fflush(stdout);
      delay(1000);
   }
   printf("\n");
}


void * clnt_connection(void *arg);
void send_message(char * message, int len);
void error_handling(char *message);
void *zig_thread(void *data);
void *blue_thread(void *data);
void status_message();
int clnt_number=0;
int clnt_socks;
pthread_mutex_t mutx;
pthread_t pth_zig,pth_blue;
int  zig_thid,blue_thid;
int Auto_flg =0, Sensor_flg = 0;
char str2[1];

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

   int status = 0;
   unsigned char prev_gas_status = 0;

   do {
   send_command(GAS_D_ID, CMD_GET, CMD_GET_GAS_STATUS);
   delay(100); //0.1sec

   prev_gas_status = gas_status;

      if(gas_status == STATUS_GAS_UNKNOWN){
         printf("GAS STATUS : UNKNOWN\n");
      }else if(gas_status == STATUS_GAS_DETECTED){
         printf("GAS STATUS : DETECTED\n");
         if (gas_cut_status == STATUS_GAS_CLOSED){
            printf("Gas Circuit Breaker is already closed\n");
            break;
         }
      }else if(gas_status == STATUS_GAS_CLEARED){
         printf("GAS STATUS : CLEARED\n");
         if (gas_cut_status == STATUS_GAS_OPENED){
            printf("Gas Circuit Breaker is already opened\n");
            break;
         }
      }

      while (TRUE)
      {

         if(prev_gas_status != gas_status){
            if(gas_status == STATUS_GAS_UNKNOWN){
               printf("GAS STATUS : UNKNOWN\n");

            }else if(gas_status == STATUS_GAS_DETECTED){
               printf("GAS STATUS : DETECTED\n");
               if(Auto_flg == 1){
                  if (gas_cut_status == STATUS_GAS_CLOSED){
                     printf("Gas Circuit Breaker is already closed\n");
                     break;
                  }
                     ack_recved = FALSE;
                     send_command(GAS_CB_ID, CMD_SET, CMD_GAS_CUT);
                     delay(100);
                     if (ack_recved){
                        gas_cut_status = STATUS_GAS_CLOSED;
                        status_message();
                        ack_recved = FALSE;
                        wait_10sec();
                     }
               }else{
                  status_message();
               }
            }else if(gas_status == STATUS_GAS_CLEARED){
               printf("GAS STATUS : CLEARED\n");
               if(Auto_flg == 1){
                  if (gas_cut_status == STATUS_GAS_OPENED){
                     printf("Gas Circuit Breaker is already opened\n");
                     break;
                  }
                  ack_recved = FALSE;
                  send_command(GAS_CB_ID, CMD_SET, CMD_GAS_OPEN);
                  delay(100);
                  if (ack_recved){
                     gas_cut_status = STATUS_GAS_OPENED;
                     status_message();
                     ack_recved = FALSE;
                     wait_10sec();
                  }
               }else{
                  status_message();
               }
            }
            prev_gas_status = gas_status;
         }
         delay(500); //0.5sec
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
   int i,groupid;
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
      if(Auto_flg == 0){
         send_command(GAS_CB_ID, CMD_SET, CMD_GAS_OPEN);
         delay(100);
         if (ack_recved){
            gas_cut_status = STATUS_GAS_OPENED;
            status_message();
            wait_10sec();
         }
      }
   }else if(!strcmp(message,"1\n")){
      if(Auto_flg == 0){
         send_command(GAS_CB_ID, CMD_SET, CMD_GAS_CUT);
         delay(100);
         if (ack_recved){
            gas_cut_status = STATUS_GAS_CLOSED;
            status_message();
            wait_10sec();
         }
      }
   }else if(!strcmp(message,"2\n")){
      Auto_flg=1;
      Sensor_flg=1;
      if(gas_status == STATUS_GAS_CLEARED){
         send_command(GAS_CB_ID, CMD_SET, CMD_GAS_OPEN);
         delay(100);
         if (ack_recved){
            gas_cut_status = STATUS_GAS_OPENED;
            status_message();
            wait_10sec();
         }
      }
   }else if(!strcmp(message,"3\n")){
      Auto_flg=0;
      Sensor_flg=0;
   }else if(!strcmp(message,"4\n")){
      Sensor_flg=1;
      status_message();
   }else if(!strcmp(message,"5\n")){
      Sensor_flg=0;
   }
   bzero(message, sizeof(message));
}
void status_message(){
   if(Sensor_flg == 1){
      if(gas_status == STATUS_GAS_DETECTED){
         if(gas_cut_status == STATUS_GAS_CLOSED)
            sprintf(str2,"0");
         else
            sprintf(str2,"1");
      }else if(gas_status == STATUS_GAS_CLEARED){
         if(gas_cut_status == STATUS_GAS_CLOSED)
            sprintf(str2,"2");
         else
            sprintf(str2,"3");
      }
   }else {
      if(gas_cut_status == STATUS_GAS_CLOSED)
         sprintf(str2,"4");
      else
         sprintf(str2,"5");
   }
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
