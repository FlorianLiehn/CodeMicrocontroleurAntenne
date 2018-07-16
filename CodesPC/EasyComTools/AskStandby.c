/*
 * AskStandby.c
 *
 *  Created on: 12 juil. 2018
 *      Author: liehnfl
 */

#include "../PcParameters.h"

int main(void){

	//init pipe
	mkfifo(PIPE_NAME, PIPE_PERMISSION);

	ARGS argument;
	computeAntennaMessage(argument.message_antenne,
			PUISSANCE_ACTIVATE,MODE_ANTENNA_STANDBY,0,0);

	printf("Connection to Emitter pipe\n");
	int pipe_port=open(PIPE_NAME, O_WRONLY);
    writeMessageToPipe(pipe_port,ID_MSG_ORDER_ANTENNA,argument);
    close(pipe_port);
	printf("Standby message written\n");
	return 0;
}
