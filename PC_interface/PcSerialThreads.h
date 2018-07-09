/*
 * PcSerialThreads.h
 *
 *  Created on: 8 juin 2018
 *      Author: liehnfl
 */

#ifndef PC_INTERFACE_PCSERIALTHREADS_H_
#define PC_INTERFACE_PCSERIALTHREADS_H_

#include "Messages/messages.h"

//PC<->microcontroller SD2
#define PC_PIN_RX PAL_LINE(GPIOA, 3)
#define PC_PIN_TX PAL_LINE(GPIOA, 2)


void startPcThreads(objects_fifo_t* log, objects_fifo_t* order,
					Trajectory* traj);

static inline int stmPcReader(uint8_t* buff,int n){
	return sdAsynchronousRead(&SD2,buff,n);}
static inline int stmPcWriter(uint8_t* buff,int n){
	return sdAsynchronousWrite(&SD2,buff,n);}


int handleIncomingMessage(objects_fifo_t*  fifo_log,objects_fifo_t* fifo_order,
						Trajectory* traj,SimpleMessage incoming_message);
int handleTrajectory(objects_fifo_t*  fifo_log,
			Trajectory* traj,SimpleMessage incoming_traj_message);
int handleCommunMessage(objects_fifo_t*  fifo_log,objects_fifo_t* fifo_order,
							SimpleMessage incoming_message);

#endif /* PC_INTERFACE_PCSERIALTHREADS_H_ */
