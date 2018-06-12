/*
 * PcSerialThreads.c
 *
 *  Created on: 8 juin 2018
 *      Author: liehnfl
 */

#include "PcSerialThreads.h"

const SerialConfig PcSerialConfig =  {
  115200,
  0,
  USART_CR2_STOP1_BITS /*| USART_CR2_LINEN*/,
  0
};

THD_WORKING_AREA(waPC_TxThread, 128);
THD_FUNCTION(PC_TxThread, arg) {

	mailbox_t*  mailbox_log  =(mailbox_t*)arg;
	msg_t msg=(msg_t)-1;
	uint8_t emit_buffer[serialMessageLength];

	chRegSetThreadName("Thread TX PC");

	int phase=0;
	palSetPad(GPIOD, GPIOD_LED6);
	while (true) {

		msg_t state = chMBFetchI(mailbox_log,&msg);

		if(state==MSG_OK){
			//send message
			union Payload_message payload={.message=*(struct log_message*)msg};
			encodePayload(payload.buffer,emit_buffer);
			sdWrite(&SD2, emit_buffer,serialMessageLength);
			//sdAsynchronousWrite(&SD2, (uint8_t*)emit_buffer,serialMessageLength);

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
		strcpy(log_test.message_antenne,"TEST MESSAGE");

		struct log_message test={
				.order=ORDER_GOTO,
				.logs =log_test,
		};

		(void)chMBPostI(mailbox_log, (msg_t)&test);


	}
}
