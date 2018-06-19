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

#define ANTENNA_MESSAGE_LENGTH 12

#define HEADER_ANTENNA "\x16" //22=0x16=SYN

#define ANTENNA_SURVIE			HEADER_ANTENNA"8400000000\r"
#define ANTENNA_DESACTIVATE		HEADER_ANTENNA"0000000000\r"
#define ANTENNA_STANDBY			HEADER_ANTENNA"8000000000\r"
#define ANTENNA_CALLAGE			HEADER_ANTENNA"8100000000\r"


//Id of each messages
enum ID_MSG{
	//50 order id max
	ID_MSG_ORDER_SURVIE,
	ID_MSG_ORDER_REINI,
	ID_MSG_ORDER_CALAGE,
	ID_MSG_ORDER_GOTO,
	ID_MSG_ORDER_ANTENNA,

	//30 error id max
	ID_MSG_ALERT_CRC_ERROR=50,
	ID_MSG_ALERT_BAD_ANTENNA_RESPONSE,

	//>128 log id max
	ID_MSG_LOG_ANTENNA_RETURN=80,
	//TODO all id message

};//Max 256 (uint8_t)

/////////////////////ARGS DEFINITION//////////////////

//angles for the goto order
typedef struct {
	uint8_t elevation;
	uint8_t azimut;
}goto_args;

//Union of all args for all messages
typedef union {
	char message_antenne[ANTENNA_MESSAGE_LENGTH];
	char date[ANTENNA_MESSAGE_LENGTH];
	goto_args ARGS_goto;
}ARGS;

//////////////////MESSAGES DEFINITION//////////////////

//message for microcontroller
typedef struct {
	uint8_t order;//fill with ORDER enum
	ARGS arguments;
}order_message;

//log or reply from microcontroller
typedef struct {
	uint8_t id;
	//TODO add here a timestamps (long)
	ARGS logs;
}log_message;

#define MaxPayloadMessageLength (int)(sizeof(log_message)/(sizeof(char)))
#define MaxSerialMessageLength (int)(1+1+MaxPayloadMessageLength+1)
							//INIT+length_Payload+[Payload]+CRC

typedef union {
	log_message message;
	char buffer[MaxPayloadMessageLength];
}Payload_message;


//////////////////MESSAGES ENCODE/DECODE//////////////////

//CRC table computation
void crcInit(void);
uint8_t ComputeCRC(uint8_t * message, int nBytes);

int PayloadLength(int id);
int encodePayload(char* payload,uint8_t* msg);

int write_message(int(*writer)(uint8_t*,int),Payload_message payload);
int read_message(int(*reader)(uint8_t*,int),uint8_t* message);

//If on microcontroller (and not on PC)
#if !(defined(_WIN32) || defined(WIN32)  ||  defined(__unix__) )
////////////////////////Thread Inputs/////////////////////
//Init args for Threads

#include "hal.h"
typedef struct {
	objects_fifo_t* fifo_log_arg;
	objects_fifo_t* fifo_order_arg;
}Fifos_args;
#endif

#endif /* MESSAGES_MESSAGES_H_ */
