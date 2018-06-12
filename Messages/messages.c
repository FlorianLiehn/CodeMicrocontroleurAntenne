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

void encodePayload(char* payload,uint8_t* msg){

	*(msg+0)=HEADER_BYTE;//Header
	*(msg+1)=Payload_message_lenght;//lenght of the message
	for(int i=0;i<serialMessageLength;i++){
		*(msg+i+2)=payload[i];
	}
	uint8_t crc=ComputeCRC((uint8_t*)payload,Payload_message_lenght);
	*(msg+serialMessageLength-1)=crc;//CRC to check the communication
}
