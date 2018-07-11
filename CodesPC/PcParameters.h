/*
 * PcParameters.h
 *
 *  Created on: 10 juil. 2018
 *      Author: liehnfl
 */

#ifndef CODESPC_PCPARAMETERS_H_
#define CODESPC_PCPARAMETERS_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "../Messages/messages.h"

#define PIPE_NAME "/tmp/fifoMessageToAntenna"
#define PIPE_PERMISSION 0666

#define PC_DEBUG

#endif /* CODESPC_PCPARAMETERS_H_ */
