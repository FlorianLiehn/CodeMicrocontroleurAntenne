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

//Interface PC<->uC config
extern const SerialConfig PcSerialConfig;

//TX PC thread
extern THD_WORKING_AREA(waPC_TxThread, 128);
THD_FUNCTION(PC_TxThread, arg);

//RX PC thread
struct RxThread_args{
	objects_fifo_t* fifo_log_arg;
	objects_fifo_t* fifo_order_arg;
};

extern THD_WORKING_AREA(waPC_RxThread, 128);
THD_FUNCTION(PC_RxThread, arg);

#define TIMEOUT TIME_MS2I(500) //Number of ms
int read_message(uint8_t* message);

struct log_message* next_message(void);

#endif /* PC_INTERFACE_PCSERIALTHREADS_H_ */
