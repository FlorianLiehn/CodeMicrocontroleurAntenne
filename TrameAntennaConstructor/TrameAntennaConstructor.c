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
	//MODE			0 to 9		A to F
	*(message+(n++))=CHAR_ZERO+puissance + (puissance/10)*( CHAR_A_OFFSET ) ;
	*(message+(n++))=CHAR_ZERO+mode;
	//X&Y
	int16_t X, Y;
	computeXYencoderFromAzEl(&X, &Y, az, el);

	//Write X
	char Pos_send[INT16_ASCCI_HEX_LENGTH+1];//+ '\0'
	snprintf(Pos_send,INT16_ASCCI_HEX_LENGTH+1,"%04X", (uint16_t)X );
	for(int i=0;i<INT16_ASCCI_HEX_LENGTH;i++)
		*(message+(n++))=Pos_send[i];
	//Write Y
	snprintf(Pos_send,INT16_ASCCI_HEX_LENGTH+1,"%04X", (uint16_t)Y );
	for(int i=0;i<INT16_ASCCI_HEX_LENGTH;i++)
		*(message+(n++))=Pos_send[i];
	//End Byte
	*(message+(n++))=TAIL_ANTENNA[0];
}
