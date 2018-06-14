/*
 * AntennaThreads.h
 *
 *  Created on: 13 juin 2018
 *      Author: liehnfl
 */

#ifndef ANTENNATHREADS_H_
#define ANTENNATHREADS_H_

#include "hal.h"

#include "Messages/messages.h"


//Interface uC<->Antenna (Port SD3: RS-422)
extern const SerialConfig AntennaSerialConfig;

//TX PC thread
extern THD_WORKING_AREA(waAntenna_TxThread, 128);
THD_FUNCTION(Antenna_TxThread, arg);


#endif /* ANTENNATHREADS_H_ */
