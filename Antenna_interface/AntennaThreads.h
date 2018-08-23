/*
 * AntennaThreads.h
 *
 *  Created on: 13 juin 2018
 *      Author: liehnfl
 */

#ifndef ANTENNATHREADS_H_
#define ANTENNATHREADS_H_

#include "Messages/messages.h"

//Time out if no 1pps receive (wait 1005 ms
#define TIMEOUT_1PPS  TIME_MS2I(1005)
#define ANTENNA_READ_TIMEOUT TIME_MS2I(25)
#define ANTICIPATION_TIME_BEFORE_TRACKING (2 + 1 + 1)
							//2s anticip +1 error RTC + 1anticp next 1pps
#define EMERGENCY_SEC_TIMEOUT 10

#define ANTENNA_PIN_RX PAL_LINE(GPIOB,11)
#define ANTENNA_PIN_TX PAL_LINE(GPIOB,10)

#define PIN_1PPS PAL_LINE(GPIOB,0)

enum STATE_ANTENNA_TRANSMISSION{
	STATE_ANTENNA_TRANSMISSION_NOMINAL,
	STATE_ANTENNA_TRANSMISSION_WAITING_TIME,
	STATE_ANTENNA_TRANSMISSION_PROCESS_TRAJ,
	STATE_ANTENNA_EMERGENCY,
};

static inline int stmAntennaReader(uint8_t* buff,int n){
	return sdReadTimeout(&SD3,buff,n,ANTENNA_READ_TIMEOUT);}
static inline int stmAntennaWriter(uint8_t* buff,int n){
	return sdWrite(&SD3,buff,n);}

void startAntennaThreads(objects_fifo_t* log, objects_fifo_t* order,
						Trajectory* traj);

int  testEmergencyStop(int *state,SimpleMessage* input_message);
void   nominalBehaviour(int *state,objects_fifo_t*  fifo_log,
		Trajectory* traj,SimpleMessage* input_message);
void   waitingBehaviour(int *state,objects_fifo_t*  fifo_log,Trajectory* traj);
void  trackingBehaviour(int *state,objects_fifo_t*  fifo_log,Trajectory* traj);
void emergencyBehaviour(int *state,objects_fifo_t*  fifo_log,
								objects_fifo_t*  fifo_order);

int readAntennaMessage(uint8_t* message,int lenght);

time_t getTimeUnixSecFromRTCTime(RTCDateTime* Time);

#endif /* ANTENNATHREADS_H_ */
