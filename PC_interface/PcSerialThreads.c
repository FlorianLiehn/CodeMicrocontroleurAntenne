/*
 * PcSerialThreads.c
 *
 *  Created on: 8 juin 2018
 *      Author: liehnfl
 */

#include "PcSerialThreads.h"

const SerialConfig pcSerialConfig =  {
  115200,
  0,
  USART_CR2_STOP1_BITS /*| USART_CR2_LINEN*/,
  0
};

THD_WORKING_AREA(waPcTxThread, 128);
static THD_FUNCTION(pcTxThread, arg) {

	objects_fifo_t*  fifo_log_arg  =(objects_fifo_t*)arg;
	void* msg;

	chRegSetThreadName("Thread TX PC");

	while (TRUE) {

		msg_t state = chFifoReceiveObjectI(fifo_log_arg,&msg);


		if(state==MSG_OK){
			//send message
			writeMessage(stmPcWriter,(SerialPayload*)msg);

			chFifoReturnObject(fifo_log_arg,msg);

		}
		chThdSleepMilliseconds(2);
	}
}

THD_WORKING_AREA(waPcRxThread, 128);
static THD_FUNCTION(pcRxThread, arg) {

	objects_fifo_t*  fifo_log_arg  =((ThreadsArgs*)arg)->fifo_log_arg;
	objects_fifo_t*  fifo_order_arg=((ThreadsArgs*)arg)->fifo_order_arg;
	Trajectory* traj_arg=((ThreadsArgs*)arg)->traj_arg;

	SerialPayload incoming_message;

	chRegSetThreadName("Thread RX PC");

	int phase=0;
	int count=0;
	palSetPad(GPIOD, GPIOD_LED3);
	while (TRUE) {
		count++;
		int status=readMessage(stmPcReader,(uint8_t*)&incoming_message.buffer);

		if(status==0){//If CRC is wrong create a log
			writeLogToFifo(fifo_log_arg,ID_MSG_ALERT_CRC_ERROR,
				incoming_message.simple_message.arguments);
		}
		else if(status>0){
			if(handleIncomingMessage(fifo_log_arg,fifo_order_arg,
					traj_arg,incoming_message.simple_message) >= 0 )
				phase=1-phase;
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

			ARGS ping;
			strncpy(ping.message_antenne,"TEST0MESSAGE",12);
			ping.message_antenne[4]+=(count/20)%10;

			writeLogToFifo(fifo_log_arg,ID_MSG_LOG_PING,
				ping);
		}

		chThdSleepMilliseconds(25);

	}
}

void startPcThreads(objects_fifo_t* log, objects_fifo_t* order,
					Trajectory* traj){
	//init port
	//SD2 = PC A2 et A3
	palSetLineMode(PC_PIN_RX, PAL_MODE_ALTERNATE(7));
	palSetLineMode(PC_PIN_TX, PAL_MODE_ALTERNATE(7));
	sdStart(&SD2, &pcSerialConfig);

	//Creates threads
	chThdCreateStatic(waPcRxThread, sizeof(waPcRxThread), NORMALPRIO, pcRxThread,
			   (void*)&(ThreadsArgs){log, order, traj });
	chThdCreateStatic(waPcTxThread, sizeof(waPcTxThread), NORMALPRIO, pcTxThread,
														   (void*)log);

}
