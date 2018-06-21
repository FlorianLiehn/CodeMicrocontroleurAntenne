/*
 * PcSerialThreads.h
 *
 *  Created on: 8 juin 2018
 *      Author: liehnfl
 */

#ifndef PC_INTERFACE_PCSERIALTHREADS_H_
#define PC_INTERFACE_PCSERIALTHREADS_H_

#include <string.h>

#include "hal.h"

#include "Messages/messages.h"

//PC<->microcontroller SD2


void StartPcThreads(objects_fifo_t* log, objects_fifo_t* order,
					Trajectory* traj);

inline int STM_PC_reader(uint8_t* buff,int n){
	return sdAsynchronousRead(&SD2,buff,n);}
inline int STM_PC_writer(uint8_t* buff,int n){
	return sdAsynchronousWrite(&SD2,buff,n);}

int HandleIncommingMessage(objects_fifo_t*  fifo_log,objects_fifo_t* fifo_order,
						Trajectory* traj,SerialPayload incoming_message);

#endif /* PC_INTERFACE_PCSERIALTHREADS_H_ */
