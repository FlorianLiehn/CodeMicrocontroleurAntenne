/*
 * AskSurvie.c
 *
 *  Created on: 11 juil. 2018
 *      Author: liehnfl
 */


#include "../PcParameters.h"


int main(void){

	SimpleMessage message;
	message.id=ID_MSG_ORDER_SURVIE;

	printf("Connection to Emitter pipe\n");
	int pipe_port=open(PIPE_NAME, O_WRONLY);
    write(pipe_port, ((SerialPayload){.simple_message=message}).buffer,
    		MAX_SERIAL_MESSAGE_LENGTH );
    close(pipe_port);
	printf("Survie message written\n");
	return 0;
}
