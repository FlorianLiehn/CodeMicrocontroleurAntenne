/*
 * ComServer.c
 *
 *  Created on: 10 juil. 2018
 *      Author: liehnfl
 */

#include "../../CodesPC/ComServer/ComServer.h"


void cancelHandler(int dummy,int*running) {
	*running = 0;
}

int main(){
	//stop procedure : Ctrl+C
	int running=1;
	void sigHandler(int dummy){cancelHandler(dummy,&running);}
	signal(SIGINT, sigHandler);


	crcInit();

	//init Thread Read Write
	pthread_t thRead,thWrite;
	if(pthread_create(&thRead , NULL,threadReaderLoger  , NULL) == -1 ||
	   pthread_create(&thWrite, NULL,threadServerEmitter, NULL) == -1) {
		perror("pthread_create");
		return EXIT_FAILURE;
	}
	//Threads running
	while(running){
		usleep(US_EXIT_LOOP);
	}
	//cancel & error
	pthread_cancel(thRead );
	pthread_cancel(thWrite);
	perror("threads finished");
}
