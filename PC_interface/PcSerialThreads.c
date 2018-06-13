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
			//sdAsynchronousWrite(&SD2, emit_buffer,serialMessageLength);

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
	union Payload_message in_message;

	chRegSetThreadName("Thread RX PC");

	int phase=0;
	int count=0;
	palSetPad(GPIOD, GPIOD_LED3);
	while (true) {
		count++;/*
		int status=read_message((uint8_t*)&in_message.buffer);

		if(status==0){
			//TODO Log CRC problem
		}
		else if(status>=0){
			phase=~phase;

		}*/


		if(phase){
			palClearPad(GPIOD, GPIOD_LED3);
			palSetPad(GPIOD, GPIOD_LED4);
		}
		else{
			palSetPad(GPIOD, GPIOD_LED3);
			palClearPad(GPIOD, GPIOD_LED4);
		}

		union ARGS log_test;
		strcpy(log_test.message_antenne,"TEST MESSAGE");

		struct log_message test={
			.order=ORDER_GOTO,
			.logs =log_test,
		};
		//if(count%500==0)
			(void)chMBPostI(mailbox_log, (msg_t)&test);

		chThdSleepMilliseconds(250);

	}
}

int read_message(uint8_t* message){
	return 0;
	uint8_t buf [serialMessageLength];

	int n=sdAsynchronousRead(&SD2,buf,1);
	if(buf[0]!=HEADER_BYTE || n!=1)
		return -1;
	return 1;
	while(n==1)
		n+=sdAsynchronousRead(&SD2,&(buf[n]),1);//read nb
	int tot=(int)(buf[1]);
	while(n<tot){
		n+=sdAsynchronousRead(&SD2,&(buf[n]),tot+3-n);
	}
	strncpy((char*)message,(char*)&(buf[2]),tot);

	uint8_t crc=ComputeCRC(message,tot);
	if(crc!=buf[tot+3-1]){
		return 0;
	}
	return 1;
}


