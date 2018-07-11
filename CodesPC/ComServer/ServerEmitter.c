/*
 * ServerEmitter.c
 *
 *  Created on: 10 juil. 2018
 *      Author: liehnfl
 */

#include "../../CodesPC/ComServer/ComServer.h"

void *threadServerEmitter(void *arg){

	printf("Emitter thread init!\n");
	int fd=*(int*)arg;

	pthread_exit(NULL);

}
