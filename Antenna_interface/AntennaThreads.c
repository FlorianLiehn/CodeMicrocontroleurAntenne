/*
 * AntennaThreads.c
 *
 *  Created on: 13 juin 2018
 *      Author: liehnfl
 */

#include "AntennaThreads.h"

//Interface uC<->Antenna (Port SD3: RS-422)
const SerialConfig AntennaSerialConfig =  {
  19200,
  0,
  USART_CR2_STOP1_BITS | USART_CR2_LINEN,
  0
};

//TX Antenna thread
THD_WORKING_AREA(waAntenna_TxThread, 128);
static THD_FUNCTION(Antenna_TxThread, arg) {

	objects_fifo_t*  fifo_log_arg  =((Fifos_args*)arg)->fifo_log_arg;
	(void)fifo_log_arg;//unused for now
	objects_fifo_t*  fifo_order_arg=((Fifos_args*)arg)->fifo_order_arg;

	void* msg;
	StampedMessage input_message;

	chRegSetThreadName("Thread TX Antenna");

	while (true) {
		msg_t state = chFifoReceiveObjectI(fifo_order_arg,&msg);

		if(state==MSG_OK){
			//free fifo
			input_message=*(StampedMessage*)msg;
			chFifoReturnObject(fifo_order_arg,msg);

			//send message
			sdAsynchronousWrite(&SD3,
					(uint8_t*)(input_message.arguments.message_antenne),
					ANTENNA_MESSAGE_LENGTH);
		}
		chThdSleepMilliseconds(2);
	}
}


//RX Antenna thread
THD_WORKING_AREA(waAntenna_RxThread, 128);
THD_FUNCTION(Antenna_RxThread, arg){
	objects_fifo_t*  fifo_log_arg  =(objects_fifo_t*)arg;

	chRegSetThreadName("Thread RX Antenna");

	StampedMessage new_log;
	while(true){

		int status=readAntennaMessage(
				(uint8_t*)&new_log.arguments.message_antenne);

		if(status==0){
			//new bad Antenna response return
			StampedMessage* new_message=(StampedMessage*)
							chFifoTakeObjectI(fifo_log_arg);
			new_log.id=ID_MSG_ALERT_BAD_ANTENNA_RESPONSE;
			*new_message=new_log;
			chFifoSendObjectI(fifo_log_arg,  (void*)new_message);

		}
		else if(status>0){
			//new Antenna log return
			StampedMessage* new_message=(StampedMessage*)
							chFifoTakeObjectI(fifo_log_arg);
			new_log.id=ID_MSG_LOG_ANTENNA_RETURN;
			*new_message=new_log;
			chFifoSendObjectI(fifo_log_arg,  (void*)new_message);
		}

		chThdSleepMilliseconds(2);
	}

}

void StartAntennaThreads(objects_fifo_t* log, objects_fifo_t* order){
	(void)order;//unused for now
	//init port
	//SD3 = Antenna (PB10 = Tx, PB11 = Rx)
	sdStart(&SD3, &AntennaSerialConfig);
	palSetPadMode(GPIOB, 10, PAL_MODE_ALTERNATE(7));
	palSetPadMode(GPIOB, 11, PAL_MODE_ALTERNATE(7));

	//Creates threads
	chThdCreateStatic(waAntenna_RxThread, sizeof(waAntenna_RxThread), NORMALPRIO, Antenna_RxThread,
														   (void*)log);
	chThdCreateStatic(waAntenna_TxThread, sizeof(waAntenna_TxThread), NORMALPRIO, Antenna_TxThread,
			   (void*)&(Fifos_args){log  ,order,});

}
