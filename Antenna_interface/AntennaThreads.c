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


//TX PC thread
THD_WORKING_AREA(waAntenna_RxThread, 128);
THD_FUNCTION(Antenna_RxThread, arg){
	objects_fifo_t*  fifo_log_arg  =(objects_fifo_t*)arg;
	(void)fifo_log_arg;//unused for now

	log_message new_log={.id=ID_MSG_LOG_ANTENNA_RETURN};
	while(true){

		int status=readAntennaMessage((uint8_t*)&new_log.logs.message_antenne);

		if(status>0){
			//new Antenna log return
			log_message* new_message=(log_message*)
							chFifoTakeObjectI(fifo_log_arg);
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

}
