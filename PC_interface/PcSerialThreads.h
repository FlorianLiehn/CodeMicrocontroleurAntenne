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


void StartPcThreads(objects_fifo_t* log, objects_fifo_t* order,
					Trajectory* traj);

static inline int STM_PC_reader(uint8_t* buff,int n){
	return sdAsynchronousRead(&SD2,buff,n);}
static inline int STM_PC_writer(uint8_t* buff,int n){
	return sdAsynchronousWrite(&SD2,buff,n);}


int HandleIncommingMessage(objects_fifo_t*  fifo_log,objects_fifo_t* fifo_order,
						Trajectory* traj,SimpleMessage incoming_message);
int HandleTrajectory(objects_fifo_t*  fifo_log,
			Trajectory* traj,SimpleMessage incoming_traj_message);
int HandleCommunMessage(objects_fifo_t*  fifo_log,objects_fifo_t* fifo_order,
							SimpleMessage incoming_message);

#endif /* PC_INTERFACE_PCSERIALTHREADS_H_ */
