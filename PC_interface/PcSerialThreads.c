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
static THD_FUNCTION(PC_TxThread, arg) {

	objects_fifo_t*  fifo_log_arg  =(objects_fifo_t*)arg;
	void* msg;

	chRegSetThreadName("Thread TX PC");

	int phase=0;
	palSetPad(GPIOD, GPIOD_LED6);
	while (true) {

		msg_t state = chFifoReceiveObjectI(fifo_log_arg,&msg);


		if(state==MSG_OK){
			//send message
			write_message(STM_PC_writer,*(SerialPayload*)msg);

			chFifoReturnObject(fifo_log_arg,msg);

			phase=1-phase;
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
static THD_FUNCTION(PC_RxThread, arg) {

	objects_fifo_t*  fifo_log_arg  =((Fifos_args*)arg)->fifo_log_arg;
	objects_fifo_t*  fifo_order_arg=((Fifos_args*)arg)->fifo_order_arg;
	SerialPayload incoming_message;

	chRegSetThreadName("Thread RX PC");

	int phase=0;
	int count=0;
	palSetPad(GPIOD, GPIOD_LED3);
	while (true) {
		count++;
		int status=read_message(STM_PC_reader,(uint8_t*)&incoming_message.buffer);

		if(status==0){//If CRC is wrong create a log
			StampedMessage* new_message=(StampedMessage*)
					chFifoTakeObjectI(fifo_log_arg);
			new_message->id=ID_MSG_ALERT_CRC_ERROR;
			strcpy(new_message->arguments.message_antenne,
 incoming_message.stamp_message.arguments.message_antenne);
			chFifoSendObjectI(fifo_log_arg,  (void*)new_message);
		}
		else if(status>0){

			if(
			incoming_message.simple_message.id>=ID_MSG_LOG_ANTENNA_RETURN){
				StampedMessage* new_message=(StampedMessage*)
						chFifoTakeObjectI(fifo_log_arg);
				new_message->id=ID_MSG_ALERT_BAD_MESSAGE_ID;
				strcpy(new_message->arguments.message_antenne,
			incoming_message.stamp_message.arguments.message_antenne);
				chFifoSendObjectI(fifo_log_arg,  (void*)new_message);
				continue;
			}

			phase=1-phase;

			StampedMessage* new_message=(StampedMessage*)
					chFifoTakeObjectI(fifo_log_arg);
			*new_message=incoming_message.stamp_message;
			chFifoSendObjectI(fifo_log_arg,  (void*)new_message);

			//Send to Antenna Executer
			SimpleMessage* new_order=(SimpleMessage*)
					chFifoTakeObjectI(fifo_order_arg);
			new_order->arguments=incoming_message.stamp_message.arguments;
			new_order->id=incoming_message.stamp_message.id;
			if( new_order->id==ID_MSG_ORDER_SURVIE){
				new_order->id= ID_MSG_ORDER_ANTENNA;
				strcpy(new_order->arguments.message_antenne,
						ANTENNA_SURVIE);
			}
			chFifoSendObjectI(fifo_order_arg,  (void*)new_order);

		}


		if(phase){
			palClearPad(GPIOD, GPIOD_LED3);
			palSetPad(GPIOD, GPIOD_LED4);
		}
		else{
			palSetPad(GPIOD, GPIOD_LED3);
			palClearPad(GPIOD, GPIOD_LED4);
		}

		if(count%20==0){
			StampedMessage* new_message=(StampedMessage*)
					chFifoTakeObjectI(fifo_log_arg);
			new_message->id=ID_MSG_LOG_PING;
			strncpy(new_message->arguments.message_antenne,
					"TEST0MESSAGE",12);
			new_message->arguments.message_antenne[4]+=(count/20)%10;
			chFifoSendObjectI(fifo_log_arg,(void*)new_message);
		}

		chThdSleepMilliseconds(25);

	}
}

void StartPcThreads(objects_fifo_t* log, objects_fifo_t* order){
	//init port
	//SD2 = PC A2 et A3
	sdStart(&SD2, &PcSerialConfig);
	palSetPadMode(GPIOA, 2, PAL_MODE_ALTERNATE(7));
	palSetPadMode(GPIOA, 3, PAL_MODE_ALTERNATE(7));

	//Creates threads
	chThdCreateStatic(waPC_RxThread, sizeof(waPC_RxThread), NORMALPRIO, PC_RxThread,
			   (void*)&(Fifos_args){log  ,order,});
	chThdCreateStatic(waPC_TxThread, sizeof(waPC_TxThread), NORMALPRIO, PC_TxThread,
														   (void*)log);

}
