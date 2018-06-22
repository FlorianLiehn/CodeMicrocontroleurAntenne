/*
 * AntennaFunctions.c
 *
 *  Created on: 14 juin 2018
 *      Author: liehnfl
 */

#include "AntennaThreads.h"

int readAntennaMessage(uint8_t* message,int lenght){
	memset(message,0,lenght);

	//Read Header Byte
	int n=sdAsynchronousRead(&SD3,&message[0],1);

	if( n!=1 )
		return -1;
	if( message[0]!=HEADER_ANTENNA[0] )
		return 0;

	sdRead(&SD3,&(message[n]),lenght-n);

	return 1;

}
