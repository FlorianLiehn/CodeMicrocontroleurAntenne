/*
 * TrameAntennaConstructor.c
 *
 *  Created on: 29 juin 2018
 *      Author: liehnfl
 */

#include "TrameAntennaConstructor.h"


static inline int checkElevation(double el){
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
	if( !checkElevation(el) )return;
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
	if(    *X>=MAX_ENCODER_X ||
		   *X<=MIN_ENCODER_X ||
		   *Y>=MAX_ENCODER_Y ||
		   *Y<=MIN_ENCODER_Y   ){
	   *Y=*X=0;
	   return ;
	}
}

void computeAntennaMessage(char*message, uint8_t puissance,uint8_t mode,
							double az, double el){
	memset(message,0,ANTENNA_MESSAGE_LENGTH);

	int n=0;
	//SYN
	*(message+(n++))=HEADER_ANTENNA[0];
	//MODE
	*(message+(n++))=CHAR_ZERO+puissance;
	*(message+(n++))=CHAR_ZERO+mode;
	//X&Y
	int32_t X, Y;
	char X_send[INT16_ASCCI_HEX_LENGTH];char Y_send[INT16_ASCCI_HEX_LENGTH];
	computeXYencoderFromAzEl((int16_t*)&X, (int16_t*)&Y, az, el);

	sprintf(X_send,"%04X", (uint16_t)(X &~ (0xFFFF0000) ) );
	sprintf(Y_send,"%04X", (uint16_t)(Y &~ (0xFFFF0000) ) );

	for(int i=0;i<INT16_ASCCI_HEX_LENGTH;i++)
		*(message+(n++))=X_send[i];
	for(int i=0;i<INT16_ASCCI_HEX_LENGTH;i++)
		*(message+(n++))=Y_send[i];

	*(message+(n++))=TAIL_ANTENNA[0];
}
