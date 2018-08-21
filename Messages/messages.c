/*
 * messages.c
 *
 *  Created on: 12 juin 2018
 *      Author: liehnfl
 */
#include "messages.h"

#define WIDTH  (8 * sizeof(uint8_t))
#define TOPBIT (1 << (WIDTH - 1))

//https://barrgroup.com/Embedded-Systems/How-To/CRC-Calculation-C-Code
static uint8_t crcTable[CRC_TABLE_LENGTH]={};
void crcInit(void){
	uint8_t  remainder;
    //Compute the remainder of each possible dividend.
    for (int dividend = 0; dividend < CRC_TABLE_LENGTH; ++dividend)
    {
        // Start with the dividend followed by zeros.
        remainder = dividend << (WIDTH - 8);
        //Perform modulo-2 division, a bit at a time.
        for (uint8_t bit = 8; bit > 0; --bit)
        {
            //Try to divide the current data bit.
            if (remainder & TOPBIT)
            {
                remainder = (remainder << 1) ^ POLYNOMIAL_CRC;
            }
            else
            {
                remainder = (remainder << 1);
            }
        }
        //Store the result into the table.
        crcTable[dividend] = remainder;
    }
}

uint8_t computeCRC(uint8_t* message, int nBytes)
{
    uint8_t data;
    uint8_t remainder = 0;
    //Divide the message by the polynomial, a byte at a time.
    for (int byte = 0; byte < nBytes; ++byte)
    {
        data = message[byte] ^ (remainder >> (WIDTH - 8));
        remainder = crcTable[data] ^ (remainder << 8);
    }
    //The final remainder is the CRC.
    return (remainder);
}

inline int getPayloadLength(int id){
	//size of Id
	int base=BASE_MESSAGE_LENGTH;
	//sizeof timestamps
	if(id >= FIRST_ERROR_ID)base=BASE_LOG_LENGTH;
	id%=ID_MSG_LOG_REEMIT_OFFSET;

	switch(id){
	case ID_MSG_ORDER_SURVIE:
	case ID_MSG_ORDER_DESACTIVATE:
	case ID_MSG_ORDER_REINI:
	case ID_MSG_ORDER_CALAGE:
	case ID_MSG_ORDER_TRAJ_REINI:
	case ID_MSG_ORDER_TRAJ_CHECK_CORRECT:
	case ID_MSG_ALERT_NO_1PPS:
	case ID_MSG_ALERT_WRONG_GPS_MESSAGE:
	case ID_MSG_ALERT_ANTENNA_EMERGENCY:
	case ID_MSG_LOG_TRAJ_BEGIN_TRAJECTORY:
	case ID_MSG_LOG_TRAJ_WAITING_TRAJECTORY:
	case ID_MSG_LOG_TRAJ_FINISH_TRAJECTORY:
	case ID_MSG_LOG_EXIT_EMERGENCY:
	case ID_MSG_ALERT_TRAJECTORY_DROPPED:
		return (sizeof(NoneArgs)/sizeof(uint8_t))+base;//Args size + base
	case ID_MSG_ORDER_TRAJ_SET_LENGTH:
		return (sizeof(TrajLengthArgs)/sizeof(uint8_t))+base;//Args size + base
	case ID_MSG_LOG_TRAJ_RESPONSE_CORRECT:
		return (sizeof(A_StateArgs)/sizeof(uint8_t))+base;//Args size + base
	case ID_MSG_ORDER_DO_TRAJ_AT_DATE:
		return (sizeof(DateArgs)/sizeof(uint8_t))+base;//Args size + base


	default:
		return sizeof(ARGS)/sizeof(uint8_t)+base;
	}

}

size_t encodePayload(uint8_t* payload,uint8_t* msg,int payload_length){

	int nb=0;
	*(msg+(nb++))=HEADER_BYTE;//Header
	*(msg+(nb++))=payload_length;//Lenght of the message
	for(int i=0;i<payload_length;i++){
		*(msg+(nb++))=payload[i];
	}
	uint8_t crc=computeCRC((uint8_t*)payload,payload_length);
	*(msg+(nb++))=crc;//CRC to check the communication
	return nb;
}

int writeMessage(read_write_callback_t writer,SerialPayload* payload){

	uint8_t emit_buffer[MAX_SERIAL_MESSAGE_LENGTH];
	int tot=encodePayload(payload->buffer,emit_buffer,
			getPayloadLength(payload->simple_message.id));
	return writer(emit_buffer,tot);
}

int readMessage(read_write_callback_t reader,uint8_t* message){

	uint8_t buf [MAX_SERIAL_MESSAGE_LENGTH];
    memset (message, 0, MAX_PAYLOAD_MESSAGE_LENGTH);
    //Read Header Byte
	int n=reader(buf,1);

	if(n!=1 || buf[0]!=HEADER_BYTE)
		return -1;
	//Read Payload lenght
	while(n==1)
		n+=reader(&(buf[n]),1);//read nb
	int tot=(int)(buf[1]);
	if(tot>MAX_PAYLOAD_MESSAGE_LENGTH)tot=MAX_PAYLOAD_MESSAGE_LENGTH;

	//Read Payload + CRC
	while(n<tot+3){
		n+=reader(&(buf[n]),tot+3-n);
	}
	for(int i=0;i<tot;i++)
		message[i]=buf[i+2];//copy even 0
	//Check CRC
	uint8_t crc=computeCRC(message,tot);

	if(crc!=buf[tot+3-1]){
		return 0;
	}
	return n;
}


#if !(defined(_WIN32) || defined(WIN32)  ||  defined(__unix__) )

void writeLogToFifo(objects_fifo_t* fifo_log,uint8_t id,ARGS args){

	StampedMessage* new_message=(StampedMessage*)
			chFifoTakeObjectI(fifo_log);

	new_message->id=id;
	new_message->arguments=args;
	//timestamps
	RTCDateTime current_time;
	rtcGetTime(&RTCD1, &current_time);
	//set millis
	for(int i=0;i<4;i++)
		new_message->date.millis[i]=(current_time.millisecond>>(i*8))&0xFF;
	new_message->date.day=current_time.day;
	new_message->date.month=current_time.month;
	//set year
	new_message->date.year=current_time.year;


	chFifoSendObjectI(fifo_log,(void*)new_message);
}

void convertDateArgs2RTCDateTime(RTCDateTime* time,DateArgs date_arg){

	time->year=date_arg.year;
	time->month=date_arg.month;
	time->day=date_arg.day;

	time->millisecond=0;
	for(int i=0;i<4;i++)
		time->millisecond+=(date_arg.millis[i])<<(8*i);


}

#endif

