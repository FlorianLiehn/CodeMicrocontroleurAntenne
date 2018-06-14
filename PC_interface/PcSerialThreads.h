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

//Interface PC<->uC config (SD2)
extern const SerialConfig PcSerialConfig;

//RX PC thread
typedef struct {
	objects_fifo_t* fifo_log_arg;
	objects_fifo_t* fifo_order_arg;
}RxThread_args;

void StartPcThread(objects_fifo_t* log, objects_fifo_t* order);

inline int STM_PC_reader(uint8_t* buff,int n){
	return sdAsynchronousRead(&SD2,buff,n);}

log_message* next_message(void);


#endif /* PC_INTERFACE_PCSERIALTHREADS_H_ */
