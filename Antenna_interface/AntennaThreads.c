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

//Reference between TX Antenna thread & EXT_1PPS_HANDLER
static thread_reference_t new_front_1pps = NULL;
void Handler1PPS(void*arg){
	(void)arg;
	chThdResumeI(&new_front_1pps, MSG_OK);
}

//TX Antenna thread
THD_WORKING_AREA(waAntenna_TxThread, 128);
static THD_FUNCTION(Antenna_TxThread, arg) {

	objects_fifo_t*  fifo_log_arg  =((Threads_args*)arg)->fifo_log_arg;
	objects_fifo_t*  fifo_order_arg=((Threads_args*)arg)->fifo_order_arg;
	Trajectory* traj_arg=((Threads_args*)arg)->traj_arg;
	(void)traj_arg;	   //unused for now

	void* msg;
	SimpleMessage input_message;

	chRegSetThreadName("Thread TX Antenna");

	while (TRUE) {

		msg_t state = chThdSuspendTimeoutS(&new_front_1pps,TIMEOUT_1PPS);
		if(state==MSG_TIMEOUT){
			WriteLogToFifo(fifo_log_arg,ID_MSG_ALERT_NO_1PPS,
				(ARGS){});
		}

		state = chFifoReceiveObjectI(fifo_order_arg,&msg);

		if(state==MSG_OK){
			//free fifo
			input_message=*(SimpleMessage*)msg;
			chFifoReturnObject(fifo_order_arg,msg);

#ifdef ECHO_ORDER
			//Re-Send order as Log message
			WriteLogToFifo(fifo_log_arg,
					input_message.id+ID_MSG_LOG_REEMIT_OFFSET,
					input_message.arguments);
#endif

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
	while(TRUE){

		int status=readAntennaMessage(
				(uint8_t*)&new_log.arguments.message_antenne);

		if(status==0){
			//new bad Antenna response return

			WriteLogToFifo(fifo_log_arg,ID_MSG_ALERT_BAD_ANTENNA_RESPONSE,
				new_log.arguments);

		}
		else if(status>0){
			//new Antenna log return
			WriteLogToFifo(fifo_log_arg,ID_MSG_LOG_ANTENNA_RETURN,
							new_log.arguments);
		}

		chThdSleepMilliseconds(2);
	}

}

void StartAntennaThreads(objects_fifo_t* log, objects_fifo_t* order,
						Trajectory* traj){
	//init port
	//SD3 = Antenna (PB10 = Tx, PB11 = Rx)
	sdStart(&SD3, &AntennaSerialConfig);
	palSetPadMode(GPIOB, 10, PAL_MODE_ALTERNATE(7));
	palSetPadMode(GPIOB, 11, PAL_MODE_ALTERNATE(7));
	//1pps Ext interuption
	palSetPadMode(	   GPIOB,0, PAL_MODE_INPUT_PULLDOWN );
	palEnablePadEventI(GPIOB,0, PAL_EVENT_MODE_RISING_EDGE);
	palSetPadCallback( GPIOB,0, Handler1PPS,NULL);

	//Creates threads
	chThdCreateStatic(waAntenna_RxThread, sizeof(waAntenna_RxThread), NORMALPRIO, Antenna_RxThread,
														   (void*)log);
	chThdCreateStatic(waAntenna_TxThread, sizeof(waAntenna_TxThread), NORMALPRIO, Antenna_TxThread,
			   (void*)&(Threads_args){log, order, traj });

}
