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
static uint8_t crcTable[256]={};
void crcInit(void){
	uint8_t  remainder;
    //Compute the remainder of each possible dividend.
    for (int dividend = 0; dividend < 256; ++dividend)
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

uint8_t ComputeCRC(uint8_t* message, int nBytes)
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

inline int GetPayloadLength(int id){
	//size of Id
	int base=1;
	//sizeof timestamps
	if(id>=FIRST_ERROR_ID)base+=sizeof(uint32_t)/sizeof(uint8_t);
	id%=ID_MSG_LOG_REEMIT_OFFSET;

	switch(id){
	case ID_MSG_ORDER_SURVIE:
	case ID_MSG_ORDER_REINI:
	case ID_MSG_ORDER_CALAGE:
	case ID_MSG_ORDER_TRAJ_REINI:
	case ID_MSG_ORDER_TRAJ_CHECK_CORRECT:
		return (sizeof(none_args)/sizeof(uint8_t))+base;//Args size + base
	case ID_MSG_ORDER_TRAJ_SET_LENGTH:
		return (sizeof(Traj_length_args)/sizeof(uint8_t))+base;//Args size + base
	case ID_MSG_LOG_TRAJ_RESPONSE_CORRECT:
		return (sizeof(A_State_args)/sizeof(uint8_t))+base;//Args size + base

	default:
		return sizeof(ARGS)/sizeof(uint8_t)+base;
	}

}

int encodePayload(uint8_t* payload,uint8_t* msg,int payload_length){

	int nb=0;
	*(msg+(nb++))=HEADER_BYTE;//Header
	*(msg+(nb++))=payload_length;//Lenght of the message
	for(int i=0;i<payload_length;i++){
		*(msg+(nb++))=payload[i];
	}
	uint8_t crc=ComputeCRC((uint8_t*)payload,payload_length);
	*(msg+(nb++))=crc;//CRC to check the communication
	return nb;
}

int write_message(int(*writer)(uint8_t*,int),SerialPayload payload){

	uint8_t emit_buffer[MaxSerialMessageLength];
	int tot=encodePayload(payload.buffer,emit_buffer,
			GetPayloadLength(payload.simple_message.id));
	return writer(emit_buffer,tot);
}

int read_message(int(*reader)(uint8_t*,int),uint8_t* message){

	uint8_t buf [MaxSerialMessageLength];
    memset (message, 0, MaxPayloadMessageLength);
    //Read Header Byte
	int n=reader(buf,1);

	if(n!=1 || buf[0]!=HEADER_BYTE)
		return -1;
	//Read Payload lenght
	while(n==1)
		n+=reader(&(buf[n]),1);//read nb
	int tot=(int)(buf[1]);
	if(tot>MaxPayloadMessageLength)tot=MaxPayloadMessageLength;

	//Read Payload + CRC
	while(n<tot+3){
		n+=reader(&(buf[n]),tot+3-n);
	}
	for(int i=0;i<tot;i++)
		message[i]=buf[i+2];//copy even 0
	//Check CRC
	uint8_t crc=ComputeCRC(message,tot);

	if(crc!=buf[tot+3-1]){
		return 0;
	}
	return n;
}


#if !(defined(_WIN32) || defined(WIN32)  ||  defined(__unix__) )

void WriteLogToFifo(objects_fifo_t* fifo_log,uint8_t id,ARGS args){

	StampedMessage* new_message=(StampedMessage*)
			chFifoTakeObjectI(fifo_log);

	new_message->id=id;
	new_message->arguments=args;
	//timestamps
	uint32_t time=chTimeI2MS(chVTGetSystemTimeX());
	for(int i=0;i<4;i++)
		new_message->timestamps[i]=(time>>(i*8))&0xFF;

	chFifoSendObjectI(fifo_log,(void*)new_message);
}

#endif

