/*
 * ReaderLoger.c
 *
 *  Created on: 10 juil. 2018
 *      Author: liehnfl
 */

#include "../../CodesPC/ComServer/ComServer.h"


void *threadReaderLoger(void *arg){

	(void)arg;//not used for now
	printf("Reader thread init!\n");

    pthread_exit(NULL);

}
