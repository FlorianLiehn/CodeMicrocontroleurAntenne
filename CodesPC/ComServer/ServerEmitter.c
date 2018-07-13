/*
 * ServerEmitter.c
 *
 *  Created on: 10 juil. 2018
 *      Author: liehnfl
 */

#include "ComServer.h"

void writeSerialMessage(int fd,SerialPayload seriMessage){
	//create a intermediar writer function
	inline int PcSerialWriter(uint8_t* buff,int n)
			{return write(fd,buff,n);}

	int tot= writeMessage(PcSerialWriter,&seriMessage);
#ifdef PC_DEBUG
	printf("Message written! length:%d\n",tot);
#endif
}


void *threadServerEmitter(void *arg){
#ifdef PC_DEBUG
	printf("Emitter thread init!\n");
#endif
	int micro_port=*(int*)arg;
	// Creating the named file(FIFO)
	mkfifo(PIPE_NAME, PIPE_PERMISSION);
	SerialPayload payload;
	int pipe_port=open(PIPE_NAME, O_RDONLY);

	if(pipe_port<0){
		fprintf(stderr,"Fail to open fifo %s",PIPE_NAME,50);
		pthread_exit(NULL);
	}

	while(1){
		memset(payload.buffer, 1, MAX_PAYLOAD_MESSAGE_LENGTH);
		//read the pipe for a new message
		if(read(pipe_port, payload.buffer, MAX_PAYLOAD_MESSAGE_LENGTH)
			!=MAX_PAYLOAD_MESSAGE_LENGTH){
		continue;
		}

#ifdef PC_DEBUG
		printf("New message from pipe id%d:%s\n\n\n",
			payload.simple_message.id,
			payload.simple_message.arguments.message_antenne);
#endif
		writeSerialMessage(micro_port,payload);
	}
	close(pipe_port);

	pthread_exit(NULL);

}
