/*
 * ServerEmitter.c
 *
 *  Created on: 10 juil. 2018
 *      Author: liehnfl
 */

#include "../../CodesPC/ComServer/ComServer.h"

void *threadServerEmitter(void *arg){

	(void)arg;//not used for now
	printf("Emitter thread init!\n");

	pthread_exit(NULL);

}
