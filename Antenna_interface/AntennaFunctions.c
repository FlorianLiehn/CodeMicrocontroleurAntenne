/*
 * AntennaFunctions.c
 *
 *  Created on: 14 juin 2018
 *      Author: liehnfl
 */

#include "AntennaThreads.h"

void  nominalBehaviour(int *state,objects_fifo_t*  fifo_log,
		Trajectory* traj,SimpleMessage* input_message){

	RTCDateTime Time;
	switch(input_message->id){

	case ID_MSG_ORDER_DO_TRAJ_AT_DATE:

		convertDateArgs2RTCDateTime(&Time,
				input_message->arguments.date);
		setTrajDate(traj,&Time);

		*state=STATE_ANTENNA_TRANSMISSION_WAITING_TIME;
		return;
	case ID_MSG_ORDER_ANTENNA:
		//send message
		sdAsynchronousWrite(&SD3,
			(uint8_t*)(input_message->arguments.message_antenne),
			ANTENNA_MESSAGE_LENGTH);
		return;
	default:
		WriteLogToFifo(fifo_log,ID_MSG_ALERT_BAD_MESSAGE_ID,
				input_message->arguments);
	}

}

void  waitingBehaviour(int *state,objects_fifo_t*  fifo_log,Trajectory* traj){

	RTCDateTime currentTime;
	rtcGetTime(&RTCD1, &currentTime);
	uint32_t current_sec=getTimeUnixSecFromRTCTime(&currentTime);

	RTCDateTime targetTime;
	getTrajDate(traj,&targetTime);
	uint32_t target_sec=getTimeUnixSecFromRTCTime(&targetTime);

	ARGS empty_args;
	if(target_sec-current_sec<=ANTICIPATION_TIME_BEFORE_TRACKING ||
			current_sec>target_sec){
		WriteLogToFifo(fifo_log,
			ID_MSG_LOG_TRAJ_RESPONSE_CORRECT,
			empty_args);
		*state=STATE_ANTENNA_TRANSMISSION_PROCESS_TRAJ;
	}
	else{
		WriteLogToFifo(fifo_log,60,empty_args);
	}

}

void trackingBehaviour(int *state,objects_fifo_t*  fifo_log,Trajectory* traj){
	(void)fifo_log;
	(void)traj;

	*state=STATE_ANTENNA_TRANSMISSION_NOMINAL;
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
