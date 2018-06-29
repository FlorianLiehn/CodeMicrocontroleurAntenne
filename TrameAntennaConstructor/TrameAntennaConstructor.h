/*
 * TrameAntennaConstructor.h
 *
 *  Created on: 29 juin 2018
 *      Author: liehnfl
 */

#ifndef TRAMEANTENNACONSTRUCTOR_H_
#define TRAMEANTENNACONSTRUCTOR_H_

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <math.h>


#define ANTENNA_MESSAGE_LENGTH 12
#define HEADER_ANTENNA "\x16" //22=0x16=SYN
#define TAIL_ANTENNA "\r" 	  //13=0x0D='\r'

//basics commands
#define ANTENNA_SURVIE			HEADER_ANTENNA"8400000000\r"
#define ANTENNA_DESACTIVATE		HEADER_ANTENNA"0000000000\r"
#define ANTENNA_REINI			HEADER_ANTENNA"8000000000\r"
#define ANTENNA_CALLAGE			HEADER_ANTENNA"8100000000\r"

#define MAX_EL_ANGLE 90
#define MIN_EL_ANGLE 10
#define AZIMUT_OFFSET 0.83

#define MAX_ENCODER_X 28235
#define MIN_ENCODER_X -MAX_ENCODER_X
#define MAX_ENCODER_Y 31435
#define MIN_ENCODER_Y -MAX_ENCODER_Y

#define ENCODER_X_ANGLE_IN_A_STEP (17./6000)
#define ENCODER_Y_ANGLE_IN_A_STEP (51./20000)

void computeXYencoderFromAzEl(int16_t* X,int16_t* Y,double az,double el);

#define INT16_ASCCI_HEX_LENGTH 4
#define CHAR_ZERO '0'
void computeAntennaMessage(char*message, uint8_t puissance,uint8_t mode,
							double az, double el);

#endif /* TRAMEANTENNACONSTRUCTOR_H_ */
