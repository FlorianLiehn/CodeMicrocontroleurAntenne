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
	switch(id){
	case ID_MSG_ORDER_SURVIE:
	case ID_MSG_ORDER_REINI:
	case ID_MSG_ORDER_CALAGE:
		return sizeof(none_args)/sizeof(uint8_t);

	default:
		return MaxPayloadMessageLength;
	}

}

int encodePayload(char* payload,uint8_t* msg,int payload_length){

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

int write_message(int(*writer)(uint8_t*,int),Payload_message payload){

	uint8_t emit_buffer[MaxSerialMessageLength];
	int tot=encodePayload(payload.buffer,emit_buffer,
			GetPayloadLength(payload.message.id));
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
	strncpy((char*)message,(char*)&(buf[2]),tot);
	//Check CRC
	uint8_t crc=ComputeCRC(message,tot);

	if(crc!=buf[tot+3-1]){
		return 0;
	}
	return 1;
}
