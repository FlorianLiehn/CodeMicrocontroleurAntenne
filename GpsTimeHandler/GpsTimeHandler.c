/*
 * GpsTimeHandler.c
 *
 *  Created on: 22 juin 2018
 *      Author: liehnfl
 */

#include "GpsTimeHandler.h"


//Interface uC<->Gps(Port SD1)
static const SerialConfig GpsSerialConfig =  {
  115200,
  0,
  USART_CR2_STOP1_BITS /*| USART_CR2_LINEN*/,
  0
};


//TX Antenna thread
THD_WORKING_AREA(waGpsThread, 128);
static THD_FUNCTION(GpsThread, arg) {

	objects_fifo_t*  fifo_log_arg  =(objects_fifo_t*)arg;
	(void)fifo_log_arg;
	while(TRUE){
		chThdSleepMilliseconds(2);
	}
}

void StartGpsThread(objects_fifo_t* log){
	//init port
	//SD1 = GPS : USART 1 (PB6 = Tx, PB7 = Rx)
	palSetLineMode(GPS_PIN_RX, PAL_MODE_ALTERNATE(7));
	palSetLineMode(GPS_PIN_TX, PAL_MODE_ALTERNATE(7));
	sdStart(&SD1, &GpsSerialConfig);

	//Creates threads
	chThdCreateStatic(waGpsThread, sizeof(waGpsThread), NORMALPRIO,
								GpsThread,(void*)log);
}
