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

#define PIN_LED_PING_BUTTON1 PAL_LINE(GPIOD, GPIOD_LED3)
#define PIN_LED_PING_BUTTON2 PAL_LINE(GPIOD, GPIOD_LED4)

#define PC_READ_TIMEOUT TIME_MS2I(25)

void startPcThreads(objects_fifo_t* log, objects_fifo_t* order,
					Trajectory* traj);

static inline int stmPcReader(uint8_t* buff,int n){
	return n=sdReadTimeout(&SD2,buff,n,PC_READ_TIMEOUT);}
static inline int stmPcWriter(uint8_t* buff,int n){
	return sdWrite(&SD2,buff,n);}


int handleIncomingMessage(objects_fifo_t*  fifo_log,objects_fifo_t* fifo_order,
						Trajectory* traj,SimpleMessage incoming_message);
int handleTrajectory(objects_fifo_t*  fifo_log,
			Trajectory* traj,SimpleMessage incoming_traj_message);
int handleCommunMessage(objects_fifo_t*  fifo_log,objects_fifo_t* fifo_order,
							SimpleMessage incoming_message);

#endif /* PC_INTERFACE_PCSERIALTHREADS_H_ */
