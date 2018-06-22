/*
 * AntennaThreads.h
 *
 *  Created on: 13 juin 2018
 *      Author: liehnfl
 */

#ifndef ANTENNATHREADS_H_
#define ANTENNATHREADS_H_

#include "Messages/messages.h"

#define TIMEOUT_1PPS  TIME_MS2I(1005)

void StartAntennaThreads(objects_fifo_t* log, objects_fifo_t* order,
						Trajectory* traj);

int readAntennaMessage(uint8_t* message);

#endif /* ANTENNATHREADS_H_ */
