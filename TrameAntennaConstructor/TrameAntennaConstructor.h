/*
 * TrameAntennaConstructor.h
 *
 *  Created on: 29 juin 2018
 *      Author: liehnfl
 */

#ifndef TRAMEANTENNACONSTRUCTOR_H_
#define TRAMEANTENNACONSTRUCTOR_H_

#include <stdio.h>
#include <stdbool.h>
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
#define ANTENNA_STATUS			HEADER_ANTENNA"D0FFFF0000\r"
#define ANTENNA_POSITION		HEADER_ANTENNA"90FFFF0000\r"

//Puissance flag
#define PUISSANCE_STATUS 		4
#define PUISSANCE_ACTIVATE 		8
#define PUISSANCE_DESACTIVATE 	0
//Azimut offset & elevation limits
#define AZIMUT_OFFSET 0.83
#define MAX_EL_ANGLE 90
#define MIN_EL_ANGLE 10
//encoder limits
#define MAX_ENCODER_X 28235
#define MIN_ENCODER_X (-MAX_ENCODER_X)
#define MAX_ENCODER_Y 31435
#define MIN_ENCODER_Y (-MAX_ENCODER_Y)
//encoder increment
#define ENCODER_X_STEP_BY_ANGLE (6000./17)
#define ENCODER_Y_STEP_BY_ANGLE (20000./51)

//
#define INT16_ASCCI_HEX_LENGTH 4
#define CHAR_ZERO '0'
#define CHAR_A_OFFSET ('A'- CHAR_ZERO -10)

//ASCII antenna mods
enum MODE_ANTENNA{
	MODE_ANTENNA_STANDBY,
	MODE_ANTENNA_CALAGE,
	MODE_ANTENNA_EPHEM,
	MODE_ANTENNA_ECARTO,
	MODE_ANTENNA_SURVIE,
	MODE_ANTENNA_SPEED,
	MODE_ANTENNA_POSITION,
};

void computeXYencoderFromAzEl(int16_t* X,int16_t* Y,double az,double el);

void computeAntennaMessage(char*message, uint8_t puissance,uint8_t mode,
							double az, double el);

#endif /* TRAMEANTENNACONSTRUCTOR_H_ */
