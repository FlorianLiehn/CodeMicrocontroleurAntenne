/*
 * AntennaThreads.h
 *
 *  Created on: 13 juin 2018
 *      Author: liehnfl
 */

#ifndef ANTENNATHREADS_H_
#define ANTENNATHREADS_H_

#include "Messages/messages.h"

//Time out if no 1pps receive
#define TIMEOUT_1PPS  TIME_MS2I(1005)
#define ANTICIPATION_TIME_BEFORE_TRACKING 2
#define EMERGENCY_SEC_TIMEOUT 10

#define ANTENNA_PIN_RX PAL_LINE(GPIOB,11)
#define ANTENNA_PIN_TX PAL_LINE(GPIOB,10)

#define PIN_1PPS PAL_LINE(GPIOB,0)

void StartAntennaThreads(objects_fifo_t* log, objects_fifo_t* order,
						Trajectory* traj);


enum STATE_ANTENNA_TRANSMISSION{
	STATE_ANTENNA_TRANSMISSION_NOMINAL,
	STATE_ANTENNA_TRANSMISSION_WAITING_TIME,
	STATE_ANTENNA_TRANSMISSION_PROCESS_TRAJ,
	STATE_ANTENNA_EMERGENCY,
};

int  testEmergencyStop(int *state,SimpleMessage* input_message);
void  nominalBehaviour(int *state,objects_fifo_t*  fifo_log,
		Trajectory* traj,SimpleMessage* input_message);
void  waitingBehaviour(int *state,objects_fifo_t*  fifo_log,Trajectory* traj);
void trackingBehaviour(int *state,objects_fifo_t*  fifo_log,Trajectory* traj);


int readAntennaMessage(uint8_t* message,int lenght);

time_t getTimeUnixSecFromRTCTime(RTCDateTime* Time);

#endif /* ANTENNATHREADS_H_ */
