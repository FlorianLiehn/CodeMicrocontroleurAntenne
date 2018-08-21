/*
 * TrameAntennaConstructor.c
 *
 *  Created on: 29 juin 2018
 *      Author: liehnfl
 */

#include "TrameAntennaConstructor.h"


static inline bool checkElevation(double el){
	return (el<MAX_EL_ANGLE &&
			el>MIN_EL_ANGLE);
}

static inline double deg2Rad(double a){
	return a*M_PI/180;
}

static inline double rad2Deg(double a){
	return a*180/M_PI;
}

void computeXYencoderFromAzEl(int16_t* X,int16_t* Y,double az,double el){

	//init X & Y
	*X=*Y=0;
	//check if elevation is correct
	if( !checkElevation(el) )
		return;
	//correct azimut
	az-=AZIMUT_OFFSET;

	//calculate X
	double x_angle=asin( cos(deg2Rad(el))*sin(deg2Rad(az)) );

	*X= (rad2Deg( x_angle ) * ENCODER_X_STEP_BY_ANGLE);

	//calculate Y
	double y_angle=acos( sin(deg2Rad(el)) / cos(x_angle) );

	*Y= (rad2Deg( y_angle ) * ENCODER_Y_STEP_BY_ANGLE);
	if( az>90 && az<270 )
		*Y=-*Y;

	//security
	if(    *X>MAX_ENCODER_X ||
		   *X<MIN_ENCODER_X ||
		   *Y>MAX_ENCODER_Y ||
		   *Y<MIN_ENCODER_Y   ){
	   *Y=*X=0;
	   return ;
	}
}

void computeAntennaMessage(char*message, uint8_t puissance,uint8_t mode,
							double az, double el){
	memset(message,0,ANTENNA_MESSAGE_LENGTH);

	//SYN
	*(message++)=HEADER_ANTENNA[0];
	//MODE			0 to 9		A to F
	*(message++)=CHAR_ZERO+puissance + (puissance/10)*( CHAR_A_OFFSET ) ;
	*(message++)=CHAR_ZERO+mode;
	//X&Y
	int16_t X, Y;
	computeXYencoderFromAzEl(&X, &Y, az, el);

	//Cast int to uint
	//     -42 = FFD6 ( Hex 2 complement )
	//Write X
	snprintf(message,INT16_ASCCI_HEX_LENGTH+1,"%04X", (uint16_t)X );
	message+=4;
	//Write Y  ( Warning '\0' is writen in the last byte )
	snprintf(message,INT16_ASCCI_HEX_LENGTH+1,"%04X", (uint16_t)Y );
	message+=4;
	//End Byte ( erase '\0' )
	*(message++)=TAIL_ANTENNA[0];
}
