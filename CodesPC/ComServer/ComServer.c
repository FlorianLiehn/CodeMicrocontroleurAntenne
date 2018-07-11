/*
 * ComServer.c
 *
 *  Created on: 10 juil. 2018
 *      Author: liehnfl
 */

#include "../../CodesPC/ComServer/ComServer.h"

int main(){
	//init Thread Read Write
    pthread_t thRead,thWrite;

    if(pthread_create(&thRead , NULL,threadReaderLoger  , NULL) == -1 ||
       pthread_create(&thWrite, NULL,threadServerEmitter, NULL) == -1) {
		perror("pthread_create");
		return EXIT_FAILURE;
    }

    //not normal issue
    pthread_join(thRead, NULL);
    pthread_join(thWrite, NULL);
    perror("threads finished");

}
