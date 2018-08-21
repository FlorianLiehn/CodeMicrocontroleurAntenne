/*
 * GpsTimeHandler.c
 *
 *  Created on: 22 juin 2018
 *      Author: liehnfl
 */

#include "GpsTimeHandler.h"


//Interface uC<->Gps(Port SD1)
static const SerialConfig gpsSerialConfig =  {
  115200,
  0,
  USART_CR2_STOP1_BITS /*| USART_CR2_LINEN*/,
  0
};

static void setFormatNMEAmessage(void){
	const  int8_t id_message=0x08;
	const int16_t pay_length=9;

	char set_RMC[16];
	int n=0;
	//header
	set_RMC[n++]=0xa0;//first  header
	set_RMC[n++]=0xa1;//second header
	//length
	set_RMC[n++]=pay_length>>8;
	set_RMC[n++]=pay_length&0xFF;

	//message
	const int offset_id_message=n;
	set_RMC[n++]=id_message;//id
	set_RMC[n++]=0;//GGA rate
	set_RMC[n++]=0;//GSA rate
	set_RMC[n++]=0;//GSV rate
	set_RMC[n++]=0;//GLL rate
	set_RMC[n++]=0;//RMC rate
	set_RMC[n++]=0;//VTG rate
	set_RMC[n++]=1;//ZDA rate -> Timming message
	set_RMC[n++]=1;//Update SRAM & FLASH

	//checksum
	set_RMC[n]=0;
	for(int i=offset_id_message;i<n;i++)
		set_RMC[n]^=set_RMC[i];
	n++;
	//ending of sequence
	set_RMC[n++]=0x0d;//first  end tail \r
	set_RMC[n++]=0x0a;//second end tail \n

	sdWrite(&SD1,(uint8_t *)set_RMC,n);
}

static uint32_t getMillisFromGpsBuffer(char*buf){
	const int offset_hours=0;
	const int offset_minutes=2;
	const int offset_seconds=4;
	const int offset_milliseconds=7;

	int h=(buf[offset_hours+0]-CHAR_ZERO)*10+
		   buf[offset_hours+1]-CHAR_ZERO;
	int m=(buf[offset_minutes+0]-CHAR_ZERO)*10+
		   buf[offset_minutes+1]-CHAR_ZERO;
	int s=(buf[offset_seconds+0]-CHAR_ZERO)*10+
		   buf[offset_seconds+1]-CHAR_ZERO;

	int ms=(buf[offset_milliseconds+0]-CHAR_ZERO)*100+
		   (buf[offset_milliseconds+1]-CHAR_ZERO)*10+
		    buf[offset_milliseconds+2]-CHAR_ZERO;

	return ((h*60+m)*60+s)*1000+ms;
}

static void setRTCfromGpsBuffer(char* buf){
	//TODO use correctly sscanf !
	const int offset_id_day=	11;
	const int offset_id_month=	14;
	const int offset_id_year=	17;

	//timestamps
	RTCDateTime current_time;
	current_time.millisecond=getMillisFromGpsBuffer(buf);
	current_time.day=(buf[offset_id_day+0]-CHAR_ZERO)*10+
					  buf[offset_id_day+1]-CHAR_ZERO;
	current_time.month=(buf[offset_id_month+0]-CHAR_ZERO)*10+
			 	 	    buf[offset_id_month+1]-CHAR_ZERO;
	current_time.year=(buf[offset_id_year+0]-CHAR_ZERO)*1000+
					  (buf[offset_id_year+1]-CHAR_ZERO)*100+
					  (buf[offset_id_year+2]-CHAR_ZERO)*10+
					   buf[offset_id_year+3]-CHAR_ZERO;
	current_time.year-=YEAR_OFFSET;
	//set the new time
	rtcSetTime(&RTCD1, &current_time);

}

//TX Antenna thread
THD_WORKING_AREA(waGpsThread, 512);
static THD_FUNCTION(gpsThread, arg) {

	objects_fifo_t*  fifo_log_arg  =(objects_fifo_t*)arg;

	char buf[ZDA_DATE_LENGTH];

	setFormatNMEAmessage();

	while(TRUE){

		memset(buf,0,ZDA_DATE_LENGTH);

		int n=sdReadTimeout(&SD1,(uint8_t *)buf,1,TIME_OUT_READ_GPS);
		if(n==1 && buf[0]=='$'){
			sdRead(&SD1,(uint8_t *)buf,gps_type_size);

			if(strncmp(buf,GPS_MESSAGE_TYPE,gps_type_size)==0){

				sdRead(&SD1,(uint8_t *)buf,ZDA_DATE_LENGTH);
				setRTCfromGpsBuffer(buf);

			}
			else{
				ARGS empty_args;
				writeLogToFifo(fifo_log_arg,ID_MSG_ALERT_WRONG_GPS_MESSAGE,
								empty_args);
			}

		}
		else if(n==0){
			ARGS empty_args;
			writeLogToFifo(fifo_log_arg,ID_MSG_ALERT_WRONG_GPS_MESSAGE,
							empty_args);
		}
		chThdSleepMilliseconds(10);
	}
}

void startGpsThread(objects_fifo_t* log){
	//init port
	//SD1 = GPS : USART 1 (PB6 = Tx, PB7 = Rx)
	palSetLineMode(GPS_PIN_RX, PAL_MODE_ALTERNATE(7));
	palSetLineMode(GPS_PIN_TX, PAL_MODE_ALTERNATE(7));
	sdStart(&SD1, &gpsSerialConfig);

	//Creates threads
	chThdCreateStatic(waGpsThread, sizeof(waGpsThread), NORMALPRIO,
								gpsThread,(void*)log);
}
