/*
 * ReaderLoger.c
 *
 *  Created on: 10 juil. 2018
 *      Author: liehnfl
 */

#include "../../CodesPC/ComServer/ComServer.h"


void *threadReaderLoger(void *arg){

	(void)arg;//not used for now
	while(1){
		printf("Reader thread init!\n");
		sleep(1);
	}

    pthread_exit(NULL);

}
