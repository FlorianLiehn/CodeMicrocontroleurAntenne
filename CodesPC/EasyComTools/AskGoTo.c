/*
 * AskGoTo.c
 *
 *  Created on: 12 juil. 2018
 *      Author: liehnfl
 */

#include "../PcParameters.h"

int main(int argc,char**argv){

	if(argc!=3){
		printf("Wrong input : ./programme az el\n");
		return EXIT_FAILURE;
	}
	double az=atof(argv[1]);
	double el=atof(argv[2]);

	//init pipe
	mkfifo(PIPE_NAME, PIPE_PERMISSION);

	SimpleMessage message;
	message.id=ID_MSG_ORDER_ANTENNA;
	computeAntennaMessage(message.arguments.message_antenne,
			PUISSANCE_ACTIVATE,MODE_ANTENNA_POSITION,az,el);

	printf("Connection to Emitter pipe\n");
	int pipe_port=open(PIPE_NAME, O_WRONLY);
	write(pipe_port, ((SerialPayload){.simple_message=message}).buffer,
    		MAX_SERIAL_MESSAGE_LENGTH );
	close(pipe_port);
	printf("Goto az:%.2f el:%.2f message written\n",az,el);
	printf("message written: %12s\n",message.arguments.message_antenne);
	return 0;
}
