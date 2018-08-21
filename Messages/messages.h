/*
 * messages.h
 *
 *  Created on: 8 juin 2018
 *      Author: liehnfl
 */

#ifndef MESSAGES_MESSAGES_H_
#define MESSAGES_MESSAGES_H_

#include "../TrameAntennaConstructor/TrameAntennaConstructor.h"

#define FIFO_BUFFER_SIZE 20
#define POLYNOMIAL_CRC 0xE1 //EYE...
#define HEADER_BYTE '('

#define FIRST_ORDER_TRAJ_ID 30
#define FIRST_ERROR_ID 50
#define FIRST_LOG_ID 80
//Id of each messages
enum ID_MSG{
	//50 order id max
	ID_MSG_ORDER_SURVIE,
	ID_MSG_ORDER_DESACTIVATE,
	ID_MSG_ORDER_REINI,
	ID_MSG_ORDER_CALAGE,
	ID_MSG_ORDER_GOTO,
	ID_MSG_ORDER_ANTENNA,
	ID_MSG_ORDER_DO_TRAJ_AT_DATE,

	ID_MSG_ORDER_TRAJ_SET_LENGTH=FIRST_ORDER_TRAJ_ID,
	ID_MSG_ORDER_TRAJ_SET_NEW_POINT,
	ID_MSG_ORDER_TRAJ_REINI,
	ID_MSG_ORDER_TRAJ_CHECK_CORRECT,

	//30 error id max
	ID_MSG_ALERT_CRC_ERROR=FIRST_ERROR_ID,
	ID_MSG_ALERT_BAD_ANTENNA_RESPONSE,
	ID_MSG_ALERT_BAD_MESSAGE_ID,
	ID_MSG_ALERT_NO_1PPS,
	ID_MSG_ALERT_WRONG_GPS_MESSAGE,
	ID_MSG_ALERT_MESSAGE_DROPPED,
	ID_MSG_ALERT_TRAJECTORY_DROPPED,
	ID_MSG_ALERT_ANTENNA_EMERGENCY,

	//>128 log id max
	ID_MSG_LOG_ANTENNA_RETURN=FIRST_LOG_ID,
	ID_MSG_LOG_PING,
	ID_MSG_LOG_TRAJ_RESPONSE_CORRECT,
	ID_MSG_LOG_TRAJ_BEGIN_TRAJECTORY,
	ID_MSG_LOG_TRAJ_WAITING_TRAJECTORY,
	ID_MSG_LOG_TRAJ_FINISH_TRAJECTORY,
	ID_MSG_LOG_EXIT_EMERGENCY,

	ID_MSG_LOG_REEMIT_OFFSET=150,
	//TODO all id message

};//Max 256 (uint8_t)

/////////////////////ARGS DEFINITION//////////////////


//angles for the goto order
typedef struct {
	uint8_t elevation;
	uint8_t azimut;
}GotoArgs;

//No args needed for some commands
typedef struct {
}NoneArgs;

#define MAX_TRAJ_LENGTH 20*60 //max= 20min
//Args needed for setting traj length
typedef struct {
	uint8_t length[2];//0=LB 1=HB
}TrajLengthArgs;

#define YEAR_OFFSET 1980
//Args needed to set the date of a trajectory & log date
typedef struct {
	uint8_t year;//since YEAR_OFFSET
	uint8_t month;
	uint8_t day;
	uint8_t millis[4];//uint8_t*4=uint32_t 0=LB 3=HB
}DateArgs;

//Args with just a bool state
typedef struct {
	uint8_t value;
}A_StateArgs;

//Union of all args for all messages
typedef union {
	char message_antenne[ANTENNA_MESSAGE_LENGTH];
	DateArgs date;
	GotoArgs ARGS_goto;
	TrajLengthArgs traj_length;
	A_StateArgs state;
	NoneArgs NO_ARGS;
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
	DateArgs date;
	ARGS arguments;
}StampedMessage;

#define BASE_MESSAGE_LENGTH (int)( (sizeof(SimpleMessage )-sizeof(ARGS))/(sizeof(char)) )
#define BASE_LOG_LENGTH     (int)( (sizeof(StampedMessage)-sizeof(ARGS))/(sizeof(char)) )

#define MAX_PAYLOAD_MESSAGE_LENGTH (int)(sizeof(StampedMessage)/(sizeof(char)))
#define MAX_SERIAL_MESSAGE_LENGTH (int)(1+1+MAX_PAYLOAD_MESSAGE_LENGTH+1)
									//INIT+length_Payload+[Payload]  +CRC

typedef union {
	StampedMessage stamp_message;
	SimpleMessage simple_message;
	uint8_t buffer[MAX_PAYLOAD_MESSAGE_LENGTH];
}SerialPayload;


//////////////////MESSAGES ENCODE/DECODE//////////////////

//CRC table computation
#define CRC_TABLE_LENGTH 256
void crcInit(void);
uint8_t computeCRC(uint8_t * message, int nBytes);

int getPayloadLength(int id);
size_t encodePayload(uint8_t* payload,uint8_t* msg,int payload_length);

//write read definition
typedef int(*read_write_callback_t)(uint8_t*,int);
int writeMessage(read_write_callback_t writer,SerialPayload* payload);
int readMessage(read_write_callback_t reader,uint8_t* message);

//If on microcontroller (and not on PC)
#if !(defined(_WIN32) || defined(WIN32)  ||  defined(__unix__) )
////////////////////////Thread Inputs/////////////////////
//Init args for Threads

//Echo every receive command
//#define ECHO_COMMAND
//echo every Order receive
#define ECHO_ORDER

#include "hal.h"
#include "ch.h"

#include "Trajectory/Trajectory.h"
typedef struct {
	objects_fifo_t* fifo_log_arg;
	objects_fifo_t* fifo_order_arg;
	Trajectory* traj_arg;
	// binary sémaphore protect the structure
	binary_semaphore_t protect_sem;
}ProtectedThreadsArgs;

void writeLogToFifo(objects_fifo_t* fifo_log,uint8_t id,ARGS args);
void convertDateArgs2RTCDateTime(RTCDateTime* time,DateArgs date_arg);
#endif

#endif /* MESSAGES_MESSAGES_H_ */
