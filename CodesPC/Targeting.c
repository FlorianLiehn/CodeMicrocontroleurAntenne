/*
 * Targeting.c
 *
 *  Created on: 16 juil. 2018
 *      Author: liehnfl
 */

#include "PcParameters.h"

void readDateFromFile(char* line,DateArgs* date){
	int year,hh,mm,ss;
	sscanf(line,"%d-%d-%d %d:%d:%d",
			&year,(int*)&date->month,(int*)&date->day,
			&hh,&mm,&ss);
	date->year=year-YEAR_OFFSET;
	int32_t millis=((hh*60+mm)*60+ss)*1000;
	for(int i=0;i<4;i++)
		date->millis[i]=(millis>>(i*8))&0xFF;
}

int trajAddPointFromLine(char* line,char * point,int mode){
	double azimut,elevation;
	sscanf(line,"%*s %*s %lf %lf",&azimut,&elevation);
	computeAntennaMessage(point,PUISSANCE_ACTIVATE,mode,
							azimut, elevation);
	return 1;
}

int loadTargetFile(char* file,char traj[MAX_TRAJ_LENGTH][ANTENNA_MESSAGE_LENGTH],
							DateArgs* date_begin){
	//initialisation
	int traj_length=0;
	FILE *ptr_file =fopen(file, "r");
	char buf[MAX_FILE_LINE_LENGTH];
	//read (first = begin date)
	while (fgets(buf,MAX_FILE_LINE_LENGTH, ptr_file)!=NULL){
		//Skip commented line
		if(buf[0]=='#')continue;
		//first point
		else if (traj_length==0){
			//calculate time
			readDateFromFile(buf,date_begin);
			//first point go to position
			traj_length+=trajAddPointFromLine(buf,traj[traj_length],
												MODE_ANTENNA_POSITION);
		}
		//calculate the antenna message
		traj_length+=trajAddPointFromLine(buf,traj[traj_length],
											MODE_ANTENNA_EPHEM);
		printf("New Message%12s\n",traj[traj_length-1]);
	}
	printf("\tTime:y:%d m:%2d d:%2d\n",
			date_begin->year+YEAR_OFFSET,date_begin->month,date_begin->day);
	return traj_length;
}

void executeTargeting(char traj[MAX_TRAJ_LENGTH][ANTENNA_MESSAGE_LENGTH],
						int length,int pipe_port,DateArgs date_begin){

	//reinitialisation: antenna
	writeMessageToPipe(pipe_port,ID_MSG_ORDER_DESACTIVATE,(ARGS){});
	sleep(TIME_BEFORE_REINI_AND_CALAGE);
	writeMessageToPipe(pipe_port,ID_MSG_ORDER_REINI,(ARGS){});
	sleep(TIME_BEFORE_REINI_AND_CALAGE);
	//calage
	writeMessageToPipe(pipe_port,ID_MSG_ORDER_CALAGE,(ARGS){});
	usleep(TIME_BEFORE2ORDERS);
	//load trajectory on microcontroleur during calage
	//reinitialisation: traj
	writeMessageToPipe(pipe_port,ID_MSG_ORDER_TRAJ_REINI,(ARGS){});
	usleep(TIME_BEFORE2ORDERS);
	//set length
	ARGS arguments;
	arguments.traj_length.length[0]=(length>>0)&0XFF;
	arguments.traj_length.length[1]=(length>>8)&0XFF;
	writeMessageToPipe(pipe_port,ID_MSG_ORDER_TRAJ_SET_LENGTH,arguments);
	usleep(TIME_BEFORE2ORDERS);
	//set traj points
	for(int i=0;i<length;i++){
		strncpy(arguments.message_antenne,traj[i],ANTENNA_MESSAGE_LENGTH);
		writeMessageToPipe(pipe_port,ID_MSG_ORDER_TRAJ_SET_NEW_POINT,arguments);
		usleep(TIME_BEFORE2ORDERS);
	}
	//set date
	writeMessageToPipe(pipe_port,ID_MSG_ORDER_DO_TRAJ_AT_DATE,
				(ARGS){.date=date_begin});
}

void targetingFromFile(char* file){
	//load data
	char traj_points[MAX_TRAJ_LENGTH][ANTENNA_MESSAGE_LENGTH];
	DateArgs traj_date;
	int traj_length=loadTargetFile(file,traj_points,&traj_date);
	//connect to the pipe
	//init pipe
	mkfifo(PIPE_NAME, PIPE_PERMISSION);
	int pipe_port=open(PIPE_NAME, O_WRONLY);
	//execute all needed order
	executeTargeting(traj_points,traj_length,pipe_port,traj_date);
	//disconnecting the pipe
    close(pipe_port);
}
