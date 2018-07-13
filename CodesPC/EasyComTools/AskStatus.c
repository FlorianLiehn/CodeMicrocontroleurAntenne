/*
 * AskStatus.c
 *
 *  Created on: 12 juil. 2018
 *      Author: liehnfl
 */

#include "../PcParameters.h"

int main(void){

	//init pipe
	mkfifo(PIPE_NAME, PIPE_PERMISSION);

	SimpleMessage message;
	message.id=ID_MSG_ORDER_ANTENNA;
	computeAntennaMessage(message.arguments.message_antenne,
			PUISSANCE_ACTIVATE+PUISSANCE_STATUS ,MODE_ANTENNA_STANDBY,0,0);


	printf("Connection to Emitter pipe\n");
	int pipe_port=open(PIPE_NAME, O_WRONLY);
    write(pipe_port, ((SerialPayload){.simple_message=message}).buffer,
    		MAX_SERIAL_MESSAGE_LENGTH );
    close(pipe_port);
	printf("Standby message written\n");
	return 0;
}
