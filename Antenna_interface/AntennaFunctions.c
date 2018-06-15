/*
 * AntennaFunctions.c
 *
 *  Created on: 14 juin 2018
 *      Author: liehnfl
 */

#include "AntennaThreads.h"

int readAntennaMessage(uint8_t* message){
	uint8_t buf [ANTENNA_MESSAGE_LENGTH];
	memset (message, 0, ANTENNA_MESSAGE_LENGTH);
	//Read Header Byte

	int n=sdAsynchronousRead(&SD3,buf,1);

	if(n!=1 || buf[0]!=(uint8_t)HEADER_ANTENNA[0])
		return -1;

	while(n<ANTENNA_MESSAGE_LENGTH){
		n+=sdAsynchronousRead(&SD3,&(buf[n]),ANTENNA_MESSAGE_LENGTH-n);
	}
	strncpy((char*)message,(char*)&(buf),ANTENNA_MESSAGE_LENGTH);

	return 1;

}
