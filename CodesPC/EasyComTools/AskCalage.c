/*
 * AskCalage.c
 *
 *  Created on: 12 juil. 2018
 *      Author: liehnfl
 */

#include "../PcParameters.h"

int main(void){
	//init pipe
	mkfifo(PIPE_NAME, PIPE_PERMISSION);

	printf("Connection to Emitter pipe\n");
	int pipe_port=open(PIPE_NAME, O_WRONLY);
    writeMessageToPipe(pipe_port,ID_MSG_ORDER_CALAGE,(ARGS){0});
    close(pipe_port);
	printf("Calage message written\n");
	return 0;
}
