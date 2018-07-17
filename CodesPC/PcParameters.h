/*
 * PcParameters.h
 *
 *  Created on: 10 juil. 2018
 *      Author: liehnfl
 */

#ifndef CODESPC_PCPARAMETERS_H_
#define CODESPC_PCPARAMETERS_H_

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "../Messages/messages.h"

#define PIPE_NAME "/tmp/fifoMessageToAntenna"
#define PIPE_PERMISSION 0666

#define MAX(x,y)  ( ( (x) > (y) ) ? x : y )

#define TIME_BEFORE_REINI_AND_CALAGE 2 //2s
#define TIME_BEFORE2ORDERS 50*1000 //50ms
#define MAX_FILE_LINE_LENGTH 1000

static inline void writeMessageToPipe(int pipe,int id,ARGS args){
	SimpleMessage mess={.id=id,.arguments=args};
    write(pipe, ((SerialPayload){.simple_message=mess}).buffer,
    				MAX_SERIAL_MESSAGE_LENGTH );
}

int trajAddPointFromLine(char* line,char * point,int mode);
int loadTargetFile(char* file,char traj[MAX_TRAJ_LENGTH][ANTENNA_MESSAGE_LENGTH],
							DateArgs* date_begin);
void executeTargeting(char traj[MAX_TRAJ_LENGTH][ANTENNA_MESSAGE_LENGTH],
						int length,int pipe_port,DateArgs date_begin);
void targetingFromFile(char* file);

#define PC_DEBUG

#endif /* CODESPC_PCPARAMETERS_H_ */
