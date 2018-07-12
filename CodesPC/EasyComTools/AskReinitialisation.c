/*
 * AskReinitialisation.c
 *
 *  Created on: 12 juil. 2018
 *      Author: liehnfl
 */

#include "../PcParameters.h"

int main(void){

	//init pipe
	mkfifo(PIPE_NAME, PIPE_PERMISSION);

	SimpleMessage message;
	//deactivate power
	message.id=ID_MSG_ORDER_DESACTIVATE;

	printf("Connection to Emitter pipe\n");
	int pipe_port=open(PIPE_NAME, O_WRONLY);
    write(pipe_port, ((SerialPayload){.simple_message=message}).buffer,
    		MAX_SERIAL_MESSAGE_LENGTH );
    close(pipe_port);
	printf("Unable power message written\n");

	sleep(2);
	//activate power
	message.id=ID_MSG_ORDER_REINI;

	printf("Connection to Emitter pipe\n");
	pipe_port=open(PIPE_NAME, O_WRONLY);
    write(pipe_port, ((SerialPayload){.simple_message=message}).buffer,
    		MAX_SERIAL_MESSAGE_LENGTH );
    close(pipe_port);
	printf("Enable power message written\n");
	return 0;
}
