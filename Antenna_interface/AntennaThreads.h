/*
 * AntennaThreads.h
 *
 *  Created on: 13 juin 2018
 *      Author: liehnfl
 */

#ifndef ANTENNATHREADS_H_
#define ANTENNATHREADS_H_

#include "hal.h"

#include "Messages/messages.h"


void StartAntennaThreads(objects_fifo_t* log, objects_fifo_t* order);

#endif /* ANTENNATHREADS_H_ */
