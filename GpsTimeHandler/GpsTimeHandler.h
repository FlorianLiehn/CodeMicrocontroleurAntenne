/*
 * GpsTimeHandler.h
 *
 *  Created on: 22 juin 2018
 *      Author: liehnfl
 */

#ifndef GPSTIMEHANDLER_H_
#define GPSTIMEHANDLER_H_

#include "Messages/messages.h"

#define TIME_BETWEEN_TIME_UPDATE 		TIME_S2I(3600)
#define TIME_BETWEEN_TIME_UPDATE_FAIL	TIME_S2I(5)

#define GPS_MESSAGE_HEADER '$'
#define GPS_MESSAGE_TYPE "GPZDA,"
static const int gps_type_size=sizeof(GPS_MESSAGE_TYPE)/sizeof(char)-1;//-'\0'

#define ZDA_DATE_LENGTH 21
#define ZDA_HOURS_LENGHT 10

//GPS<->microcontroller SD1
#define GPS_PIN_RX PAL_LINE(GPIOB, 7)
#define GPS_PIN_TX PAL_LINE(GPIOB, 6)

void startGpsThread(objects_fifo_t* log);


#endif /* GPSTIMEHANDLER_H_ */
