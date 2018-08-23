/*
 * AntennaThreads.c
 *
 *  Created on: 13 juin 2018
 *      Author: liehnfl
 */

#include "AntennaThreads.h"

//Interface uC<->Antenna (Port SD3: RS-422)
const SerialConfig antennaSerialConfig =  {
  19200,
  0,
  USART_CR2_STOP1_BITS | USART_CR2_LINEN,
  0
};

//TX Antenna thread
THD_WORKING_AREA(waAntennaTxThread, 512);
static THD_FUNCTION(antennaTxThread, arg) {
	//initialisation
	ProtectedThreadsArgs *ptarg = (ProtectedThreadsArgs*)arg;
	objects_fifo_t*  fifo_log_arg  =ptarg->fifo_log_arg;
	objects_fifo_t*  fifo_order_arg=ptarg->fifo_order_arg;
	Trajectory* traj_arg=ptarg->traj_arg;
	//free the semaphore
	chBSemSignal(& ptarg->protect_sem);

	int state=STATE_ANTENNA_TRANSMISSION_NOMINAL;

	void* msg;
	SimpleMessage input_message;

	chRegSetThreadName("Thread TX Antenna");

	while (TRUE) {

		//1Hz rate + GPS synchro
		msg_t msg_state = palWaitLineTimeout(PIN_1PPS,TIMEOUT_1PPS);
		if(msg_state==MSG_TIMEOUT){
			writeLogToFifo(fifo_log_arg,ID_MSG_ALERT_NO_1PPS,
				(ARGS){0});
		}

		//read fifo order
		msg_state = chFifoReceiveObjectTimeout(fifo_order_arg,&msg,
													TIME_IMMEDIATE);

		//process message
		if(msg_state==MSG_OK){
			//free fifo
			input_message=*(SimpleMessage*)msg;
			chFifoReturnObject(fifo_order_arg,msg);

#ifdef ECHO_ORDER
			//Re-Send order as Log message
			writeLogToFifo(fifo_log_arg,
					input_message.id+ID_MSG_LOG_REEMIT_OFFSET,
					input_message.arguments);
#endif

			if(testEmergencyStop(&state,&input_message)<=0){
				writeLogToFifo(fifo_log_arg,ID_MSG_ALERT_ANTENNA_EMERGENCY,
						input_message.arguments);
			}
			else if(state==STATE_ANTENNA_TRANSMISSION_NOMINAL){
				nominalBehaviour(&state,fifo_log_arg,traj_arg,&input_message);
			}
			else{
				writeLogToFifo(fifo_log_arg,ID_MSG_ALERT_MESSAGE_DROPPED,
						input_message.arguments);
			}
		}

		switch(state){
		case STATE_ANTENNA_TRANSMISSION_WAITING_TIME:
			waitingBehaviour(&state,fifo_log_arg,traj_arg);
			break;
		case STATE_ANTENNA_TRANSMISSION_PROCESS_TRAJ:
			trackingBehaviour(&state,fifo_log_arg,traj_arg);
			break;
		case STATE_ANTENNA_EMERGENCY:
			emergencyBehaviour(&state,fifo_log_arg,fifo_order_arg);
			break;
		}

	}
}


//RX Antenna thread
THD_WORKING_AREA(waAntennaRxThread, 512);
THD_FUNCTION(antennaRxThread, arg){
	objects_fifo_t*  fifo_log_arg  =(objects_fifo_t*)arg;

	chRegSetThreadName("Thread RX Antenna");

	StampedMessage new_log;
	while(TRUE){

		int status=readAntennaMessage(
				(uint8_t*)&new_log.arguments.message_antenne,
				ANTENNA_MESSAGE_LENGTH);

		if(status==0){
			//new bad Antenna response return

			writeLogToFifo(fifo_log_arg,ID_MSG_ALERT_BAD_ANTENNA_RESPONSE,
				new_log.arguments);

		}
		else if(status>0){
			//new Antenna log return
			writeLogToFifo(fifo_log_arg,ID_MSG_LOG_ANTENNA_RETURN,
							new_log.arguments);
		}
	}

}

void startAntennaThreads(objects_fifo_t* log, objects_fifo_t* order,
						Trajectory* traj){
	//init port
	//SD3 = Antenna (PB10 = Tx, PB11 = Rx)
	palSetLineMode(ANTENNA_PIN_RX, PAL_MODE_ALTERNATE(7));
	palSetLineMode(ANTENNA_PIN_TX, PAL_MODE_ALTERNATE(7));
	sdStart(&SD3, &antennaSerialConfig);
	//1pps Ext interuption
	palSetLineMode(	    PIN_1PPS, PAL_MODE_INPUT_PULLDOWN );
	palEnableLineEventI(PIN_1PPS, PAL_EVENT_MODE_RISING_EDGE);

	//Creates threads
	//init Rx thread
	chThdCreateStatic(waAntennaRxThread, sizeof(waAntennaRxThread), NORMALPRIO, antennaRxThread,
			   (void*)log);

	//init Tx thread
	//args
	ProtectedThreadsArgs th_args={
		.fifo_log_arg=log,.fifo_order_arg=order,.traj_arg=traj,
		.protect_sem= _BSEMAPHORE_DATA(th_args.protect_sem, 1)
	};
	//thread creation
	chThdCreateStatic(waAntennaTxThread, sizeof(waAntennaTxThread), NORMALPRIO, antennaTxThread,
			   (void*)& th_args);
	//wait the initialation of Tx Thread
	chBSemWait(&th_args.protect_sem);

}
