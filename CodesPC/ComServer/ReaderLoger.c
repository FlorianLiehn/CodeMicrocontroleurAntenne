/*
 * ReaderLoger.c
 *
 *  Created on: 10 juil. 2018
 *      Author: liehnfl
 */

#include "../../CodesPC/ComServer/ComServer.h"


void *threadReaderLoger(void *arg){

	printf("Reader thread init!\n");
	int fd=*(int*)arg;
	//running
	while(1){
		printf("Reader thread fd:%d!\n",fd);
		sleep(1);
	}

	pthread_exit(NULL);

}
