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
	ID_MSG_ORDER_TRAJ_SET_LENGTH,
	ID_MSG_ORDER_TRAJ_SET_NEW_POINT,
	ID_MSG_ORDER_TRAJ_REINI,
	ID_MSG_ORDER_TRAJ_CHECK_CORRECT,

	//30 error id max
	ID_MSG_ALERT_CRC_ERROR=50,
	ID_MSG_ALERT_BAD_ANTENNA_RESPONSE,
	ID_MSG_ALERT_BAD_MESSAGE_ID,

	//>128 log id max
	ID_MSG_LOG_ANTENNA_RETURN=80,
	ID_MSG_LOG_PING,
	ID_MSG_LOG_TRAJ_RESPONSE_CORRECT,

	ID_MSG_LOG_REEMIT_OFFSET=150,
	//TODO all id message

};//Max 256 (uint8_t)

/////////////////////ARGS DEFINITION//////////////////


//angles for the goto order
typedef struct {
	uint8_t elevation;
	uint8_t azimut;
}goto_args;

//No args needed for some commands
typedef struct {
}none_args;

//Args needed for setting traj length
typedef struct {
	uint8_t length[2];//0=LB 1=HB
}Traj_length_args;

//Args with just a bool state
typedef struct {
	uint8_t value;
}A_State_args;

//Union of all args for all messages
typedef union {
	char message_antenne[ANTENNA_MESSAGE_LENGTH];
	char date[ANTENNA_MESSAGE_LENGTH];
	goto_args ARGS_goto;
	Traj_length_args traj_length;
	A_State_args state;
	none_args NO_ARGS;
}ARGS;

//////////////////MESSAGES DEFINITION//////////////////

//message for microcontroller
typedef struct {
	uint8_t id;//fill with ORDER enum
	ARGS arguments;
}SimpleMessage;

//log or reply from microcontroller
typedef struct {
	uint8_t id;//fill with ORDER enum
	uint8_t timestamps[4];//uint8_t*4=uint32_t 0=LB 3=HB
	ARGS arguments;
}StampedMessage;

#define MaxPayloadMessageLength (int)(sizeof(StampedMessage)/(sizeof(char)))
#define MaxSerialMessageLength (int)(1+1+MaxPayloadMessageLength+1)
							//INIT+length_Payload+[Payload]+CRC

typedef union {
	StampedMessage stamp_message;
	SimpleMessage simple_message;
	uint8_t buffer[MaxPayloadMessageLength];
}SerialPayload;


//////////////////MESSAGES ENCODE/DECODE//////////////////

//CRC table computation
void crcInit(void);
uint8_t ComputeCRC(uint8_t * message, int nBytes);

int GetPayloadLength(int id);
int encodePayload(uint8_t* payload,uint8_t* msg,int payload_length);

int write_message(int(*writer)(uint8_t*,int),SerialPayload payload);
int read_message(int(*reader)(uint8_t*,int),uint8_t* message);

//If on microcontroller (and not on PC)
#if !(defined(_WIN32) || defined(WIN32)  ||  defined(__unix__) )
////////////////////////Thread Inputs/////////////////////
//Init args for Threads

#include "hal.h"

#include "Trajectory/Trajectory.h"
typedef struct {
	objects_fifo_t* fifo_log_arg;
	objects_fifo_t* fifo_order_arg;
	Trajectory* traj_arg;
}Threads_args;

void WriteLogToFifo(objects_fifo_t* fifo_log,uint8_t id,ARGS args);
#endif

#endif /* MESSAGES_MESSAGES_H_ */
