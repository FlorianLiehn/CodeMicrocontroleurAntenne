/*
 * ComServer.h
 *
 *  Created on: 10 juil. 2018
 *      Author: liehnfl
 */

#ifndef CODESPC_COMSERVER_COMSERVER_H_
#define CODESPC_COMSERVER_COMSERVER_H_

#include <pthread.h>
#include <signal.h>

#include "../../CodesPC/PcParameters.h"

#define MICROCONTROLER_PORT "/dev/ttyACM0"
#define US_EXIT_LOOP 1000*500

//define 2 threads Read Write
void *threadReaderLoger(void *arg);
void *threadServerEmitter(void *arg);

#endif /* CODESPC_COMSERVER_COMSERVER_H_ */
