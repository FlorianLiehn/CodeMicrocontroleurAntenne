/*
 * ReaderLoger.c
 *
 *  Created on: 10 juil. 2018
 *      Author: liehnfl
 */

#include "../../CodesPC/ComServer/ComServer.h"

void replaceInString(char*buff,char old,char new,int length){
	for(int i=0;i<length;i++)
		if(buff[i]==old)
			buff[i]=new;
}

void print_time(uint32_t millis){
	int ms=millis%1000;
	millis/=1000;

	int s=millis%60;
	millis/=60;

	int m=millis%60;
	millis/=60;

	int h= millis;

	printf("\t%2d:%2d:%2d.%3d\n",h,m,s,ms);
}

void print_log_message(StampedMessage message){
	int args_length=getPayloadLength(message.id);
	args_length-=8;

	char log_ascci[args_length];
	strncpy(log_ascci,message.arguments.message_antenne,
			args_length);
	replaceInString(log_ascci,'\r','\n',args_length);
	replaceInString(log_ascci,'\0','o',args_length);

	printf("Message id:%02d:",message.id%ID_MSG_LOG_REEMIT_OFFSET);
	for(int i=0;i<args_length;i++)printf("%c",log_ascci[i]);

	uint32_t millis=0;
	for(int i=0;i<4;i++)millis+=message.date.millis[i]<<(8*i);

	printf("\tTime:y:%d m:%2d d:%2d",
 message.date.year+YEAR_OFFSET,message.date.month,message.date.day);
	print_time(millis);

}

void *threadReaderLoger(void *arg){

	printf("Reader thread init!\n");
	int fd=*(int*)arg;
	//create a intermediar reader function
	inline int PcSerialReader(uint8_t* buff,int n)
		{return read(fd,buff,n);}

	SerialPayload message;
	//running
	while(1){
		int state=readMessage(PcSerialReader,message.buffer);
		if(state >= 0){
			print_log_message(message.stamp_message);
			//TODO log the message To file
			//TODO Same if error with alert
			if(state==0)printf("\tERROR ON CRC\n");
		}
	}

	pthread_exit(NULL);

}
