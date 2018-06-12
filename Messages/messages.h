/*
 * messages.h
 *
 *  Created on: 8 juin 2018
 *      Author: liehnfl
 */

#ifndef MESSAGES_MESSAGES_H_
#define MESSAGES_MESSAGES_H_

#include <stdint.h>

#define POLYNOMIAL_CRC 0xE1 //EYE...
#define HEADER_BYTE '('


//Id of each messages
enum ORDER{
	ORDER_SURVIE,
	ORDER_REINI,
	ORDER_CALAGE,
	ORDER_GOTO,
	//TODO all orders
};

/////////////////////ARGS DEFINITION//////////////////

//angles for the goto order
struct goto_args{
	uint8_t elevation;
	uint8_t azimut;
};

//Union of all args for all messages
union ARGS{
	char message_antenne[12];
	char date[12];
	struct goto_args ARGS_goto;
};

//////////////////MESSAGES DEFINITION//////////////////

//message for microcontroler
struct order_message{
	uint8_t order;//fill with ORDER enum
	union ARGS arguments;
};

//log or reply from microcontroler
struct log_message{
	uint8_t order;
	union ARGS logs;
};

#define Payload_message_lenght (sizeof(struct log_message)/(sizeof(char)))
#define serialMessageLength (int)(1+1+Payload_message_lenght+1)//INIT+nb+[Payload]+CRC

union Payload_message{
	struct log_message 	message;
	char buffer[Payload_message_lenght];
};


//////////////////MESSAGES ENCODE/DECODE//////////////////

//CRC computation
void crcInit(void);

void encodePayload(char* payload,uint8_t* msg);
uint8_t ComputeCRC(uint8_t * message, int nBytes);

#endif /* MESSAGES_MESSAGES_H_ */
