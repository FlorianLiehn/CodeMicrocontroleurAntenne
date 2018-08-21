/*
 * AskSurvie.c
 *
 *  Created on: 11 juil. 2018
 *      Author: liehnfl
 */

#include "../PcParameters.h"

int main(void){
	//init pipe
	mkfifo(PIPE_NAME, PIPE_PERMISSION);

	SimpleMessage message;
	message.id=ID_MSG_ORDER_SURVIE;
	memset(message.arguments.message_antenne,0,ANTENNA_MESSAGE_LENGTH);

	printf("Connection to Emitter pipe\n");
	int pipe_port=open(PIPE_NAME, O_WRONLY);
    writeMessageToPipe(pipe_port,ID_MSG_ORDER_SURVIE,(ARGS){0});
    close(pipe_port);
	printf("Survie message written\n");
	return 0;
}
