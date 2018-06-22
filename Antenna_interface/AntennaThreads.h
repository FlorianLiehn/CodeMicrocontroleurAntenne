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

#define ANTENNA_PIN_RX PAL_LINE(GPIOB,11)
#define ANTENNA_PIN_TX PAL_LINE(GPIOB,10)

#define PIN_1PPS PAL_LINE(GPIOB,0)

void StartAntennaThreads(objects_fifo_t* log, objects_fifo_t* order,
						Trajectory* traj);

int readAntennaMessage(uint8_t* message,int lenght);

#endif /* ANTENNATHREADS_H_ */
