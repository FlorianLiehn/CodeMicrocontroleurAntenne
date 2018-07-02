/*
 * AntennaFunctions.c
 *
 *  Created on: 14 juin 2018
 *      Author: liehnfl
 */

#include "AntennaThreads.h"

int  testEmergencyStop(int *state,SimpleMessage* input_message){
	if(input_message->id==ID_MSG_ORDER_ANTENNA &&
		strncmp(input_message->arguments.message_antenne,
				ANTENNA_SURVIE,
				ANTENNA_MESSAGE_LENGTH) == 0 ){
		//send message
		sdAsynchronousWrite(&SD3,
			(uint8_t*)(input_message->arguments.message_antenne),
			ANTENNA_MESSAGE_LENGTH);
		*state=STATE_ANTENNA_EMERGENCY;
		return -1;
	}
	return 1;
}

void  nominalBehaviour(int *state,objects_fifo_t*  fifo_log,
		Trajectory* traj,SimpleMessage* input_message){

	RTCDateTime time;
	switch(input_message->id){

	case ID_MSG_ORDER_DO_TRAJ_AT_DATE:

		convertDateArgs2RTCDateTime(&time,
				input_message->arguments.date);
		//prepare antenna
		setTrajDate(traj,&time);
		char first_target[ANTENNA_MESSAGE_LENGTH];
		trajPrepareTargeting(traj,first_target);
		//set in position
		sdAsynchronousWrite(&SD3,(uint8_t*)first_target,ANTENNA_MESSAGE_LENGTH);

		*state=STATE_ANTENNA_TRANSMISSION_WAITING_TIME;
		return;
	case ID_MSG_ORDER_ANTENNA:
		//send message
		sdAsynchronousWrite(&SD3,
			(uint8_t*)(input_message->arguments.message_antenne),
			ANTENNA_MESSAGE_LENGTH);
		return;
	default:
		writeLogToFifo(fifo_log,ID_MSG_ALERT_BAD_MESSAGE_ID,
				input_message->arguments);
	}

}

void  waitingBehaviour(int *state,objects_fifo_t*  fifo_log,Trajectory* traj){

	RTCDateTime current_time;
	rtcGetTime(&RTCD1, &current_time);
	uint32_t current_sec=getTimeUnixSecFromRTCTime(&current_time);

	RTCDateTime target_time;
	getTrajDate(traj,&target_time);
	uint32_t target_sec=getTimeUnixSecFromRTCTime(&target_time);

	ARGS empty_args;
	if(current_sec>target_sec){//too late
		writeLogToFifo(fifo_log,
				ID_MSG_ALERT_TRAJECTORY_DROPPED,
			empty_args);
		*state=STATE_ANTENNA_TRANSMISSION_NOMINAL;
	}
	else if(target_sec-current_sec<=ANTICIPATION_TIME_BEFORE_TRACKING){
		writeLogToFifo(fifo_log,
				ID_MSG_LOG_TRAJ_BEGIN_TRAJECTORY,
			empty_args);
		//change state
		*state=STATE_ANTENNA_TRANSMISSION_PROCESS_TRAJ;
	}
	else{
		writeLogToFifo(fifo_log,ID_MSG_LOG_TRAJ_WAITING_TRAJECTORY,empty_args);
	}

}

void trackingBehaviour(int *state,objects_fifo_t*  fifo_log,Trajectory* traj){

	char target[ANTENNA_MESSAGE_LENGTH];
	trajGetNextTarget(traj,target);

	//execute targeting
	sdAsynchronousWrite(&SD3,(uint8_t*)target,ANTENNA_MESSAGE_LENGTH);

	if( trajCheckCorrectLength(traj) ){
		ARGS empty_args;
		writeLogToFifo(fifo_log,ID_MSG_LOG_TRAJ_FINISH_TRAJECTORY,empty_args);

		trajInit(traj);
		*state=STATE_ANTENNA_TRANSMISSION_NOMINAL;
	}
}

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

time_t getTimeUnixSecFromRTCTime(RTCDateTime* Time){
	struct tm tim;
	rtcConvertDateTimeToStructTm(Time, &tim, NULL);
	return mktime(&tim);
}
