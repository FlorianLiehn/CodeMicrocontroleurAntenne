/*
 * TxThread.h
 *
 *  Created on: 8 juin 2018
 *      Author: liehnfl
 */

#ifndef PC_INTERFACE_PCSERIALTHREADS_H_
#define PC_INTERFACE_PCSERIALTHREADS_H_

#include <string.h>

#include "Messages/messages.h"

const SerialConfig PcSerialConfig =  {
  115200,
  0,
  USART_CR2_STOP1_BITS /*| USART_CR2_LINEN*/,
  0
};


THD_WORKING_AREA(waPC_TxThread, 128);
THD_FUNCTION(PC_TxThread, arg) {

	mailbox_t*  mailbox_log  =(mailbox_t*)arg;

	chRegSetThreadName("Thread TX PC");
	int phase=0;
	palSetPad(GPIOD, GPIOD_LED6);
	msg_t msg=(msg_t)-1;
	while (true) {

		msg_t state = chMBFetchI(mailbox_log,&msg);

		if(state==MSG_OK){
			//send message
			struct log_message log=*(struct log_message*)msg;
			union Payload_message payload={.message=log};
			sdAsynchronousWrite(&SD2, payload.buffer,
				sizeof(payload.buffer)/sizeof(uint8_t));

			phase=~phase;
			if(phase){
				palClearPad(GPIOD, GPIOD_LED6);
				palSetPad(GPIOD, GPIOD_LED5);
			}
			else{
				palSetPad(GPIOD, GPIOD_LED6);
				palClearPad(GPIOD, GPIOD_LED5);
			}
		}
		chThdSleepMilliseconds(2);
	}
}

struct RxThread_args{
	mailbox_t* mailbox_log_arg;
	mailbox_t* mailbox_order_arg;
}RxThread_args;

THD_WORKING_AREA(waPC_RxThread, 128);
THD_FUNCTION(PC_RxThread, arg) {

	mailbox_t*  mailbox_log  =((struct RxThread_args*)arg)->mailbox_log_arg;
	//mailbox_t*  mailbox_order=((struct RxThread_args*)arg)->mailbox_order_arg;

	chRegSetThreadName("Thread RX PC");
	while (true) {
		palSetPad(GPIOD, GPIOD_LED3);
		chThdSleepMilliseconds(250);
		palClearPad(GPIOD, GPIOD_LED3);

		palSetPad(GPIOD, GPIOD_LED4);
		chThdSleepMilliseconds(250);
		palClearPad(GPIOD, GPIOD_LED4);

		union ARGS log_test;
		strcpy(log_test.message_antenne,"TESTMESSAGE\n");

		struct log_message test={
				.order=ORDER_CALAGE,
				.logs =log_test,
		};

		(void)chMBPostI(mailbox_log, (msg_t)&test);


	}
}

#endif /* PC_INTERFACE_PCSERIALTHREADS_H_ */
