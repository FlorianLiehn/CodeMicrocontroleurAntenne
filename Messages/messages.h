/*
 * messages.h
 *
 *  Created on: 8 juin 2018
 *      Author: liehnfl
 */

#ifndef MESSAGES_MESSAGES_H_
#define MESSAGES_MESSAGES_H_

#include <stdint.h>
#include <string.h>

#define FIFO_BUFFER_SIZE 20
#define POLYNOMIAL_CRC 0xE1 //EYE...
#define HEADER_BYTE '('


//Id of each messages
enum ID_MSG{
	ID_MSG_ORDER_SURVIE,
	ID_MSG_ORDER_REINI,
	ID_MSG_ORDER_CALAGE,
	ID_MSG_ORDER_GOTO,
	ID_MSG_ORDER_ANTENNA,

	ID_MSG_ALERT_CRC_ERROR=50,
	//TODO all id message

};

/////////////////////ARGS DEFINITION//////////////////

//angles for the goto order
typedef struct {
	uint8_t elevation;
	uint8_t azimut;
}goto_args;

//Union of all args for all messages
typedef union {
	char message_antenne[12];
	char date[12];
	goto_args ARGS_goto;
}ARGS;

//////////////////MESSAGES DEFINITION//////////////////

//message for microcontroler
typedef struct {
	uint8_t order;//fill with ORDER enum
	ARGS arguments;
}order_message;

//log or reply from microcontroler
typedef struct {
	uint8_t id;
	ARGS logs;
}log_message;

#define Payload_message_lenght (sizeof(log_message)/(sizeof(char)))
#define serialMessageLength (int)(1+1+Payload_message_lenght+1)//INIT+nb+[Payload]+CRC

typedef union {
	log_message message;
	char buffer[Payload_message_lenght];
}Payload_message;


//////////////////MESSAGES ENCODE/DECODE//////////////////

//CRC computation
void crcInit(void);

void encodePayload(char* payload,uint8_t* msg);
uint8_t ComputeCRC(uint8_t * message, int nBytes);
int read_message(int(*reader)(uint8_t*,int),uint8_t* message);

#endif /* MESSAGES_MESSAGES_H_ */
