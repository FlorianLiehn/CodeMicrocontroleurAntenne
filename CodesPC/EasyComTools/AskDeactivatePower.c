/*
 * AskDeactivatePower.c
 *
 *  Created on: 12 juil. 2018
 *      Author: liehnfl
 */

#include "../PcParameters.h"

int main(void){
	//init pipe
	mkfifo(PIPE_NAME, PIPE_PERMISSION);

	SimpleMessage message;
	message.id=ID_MSG_ORDER_DESACTIVATE;
	memset(message.arguments.message_antenne,0,ANTENNA_MESSAGE_LENGTH);

	printf("Connection to Emitter pipe\n");
	int pipe_port=open(PIPE_NAME, O_WRONLY);
    write(pipe_port, ((SerialPayload){.simple_message=message}).buffer,
    		MAX_SERIAL_MESSAGE_LENGTH );
    close(pipe_port);
	printf("Disable power message written\n");
	return 0;
}

