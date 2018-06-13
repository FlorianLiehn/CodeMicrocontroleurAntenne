/*
 * PcSerialThreads.h
 *
 *  Created on: 8 juin 2018
 *      Author: liehnfl
 */

#ifndef PC_INTERFACE_PCSERIALTHREADS_H_
#define PC_INTERFACE_PCSERIALTHREADS_H_

#include <string.h>

#include "ch.h"
#include "hal.h"
#include "rt_test_root.h"
#include "oslib_test_root.h"

#include "Messages/messages.h"

//Interface PC<->uC config
extern const SerialConfig PcSerialConfig;

//TX PC thread
extern THD_WORKING_AREA(waPC_TxThread, 128);
THD_FUNCTION(PC_TxThread, arg);


//RX PC thread
struct RxThread_args{
	mailbox_t* mailbox_log_arg;
	mailbox_t* mailbox_order_arg;
};

extern THD_WORKING_AREA(waPC_RxThread, 128);
THD_FUNCTION(PC_RxThread, arg);

#define TIMEOUT TIME_MS2I(500) //Number of ms
int read_message(uint8_t* message);
#endif /* PC_INTERFACE_PCSERIALTHREADS_H_ */
