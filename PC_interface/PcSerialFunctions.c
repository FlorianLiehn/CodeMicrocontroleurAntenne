/*
 * PcSerialFunctions.c
 *
 *  Created on: 20 juin 2018
 *      Author: liehnfl
 */

#include "PcSerialThreads.h"

int handleIncommingMessage(objects_fifo_t*  fifo_log,objects_fifo_t* fifo_order,
						Trajectory* traj,SimpleMessage incoming_message){

	//Wrong ID ( error or log )
	if(incoming_message.id >= FIRST_ERROR_ID){

		writeLogToFifo(fifo_log,ID_MSG_ALERT_BAD_MESSAGE_ID,
			incoming_message.arguments);
		return -1;
	}

#ifdef ECHO_COMMAND
	//Re-Send command as Log message
	writeLogToFifo(fifo_log,
			incoming_message.id+ID_MSG_LOG_REEMIT_OFFSET,
		incoming_message.arguments);
#endif

	//Handle Trajectory
	if(incoming_message.id >= FIRST_ORDER_TRAJ_ID){
		return  handleTrajectory(fifo_log,traj,incoming_message);
	}

	return handleCommunMessage(fifo_log,fifo_order,incoming_message);
}


int handleTrajectory(objects_fifo_t*  fifo_log,
			Trajectory* traj,SimpleMessage incoming_traj_message){

	//TODO handle lock when On

	switch(incoming_traj_message.id){
	case ID_MSG_ORDER_TRAJ_SET_LENGTH:
		trajSetLength(traj,
				incoming_traj_message.arguments.traj_length.length[0]+
			   (incoming_traj_message.arguments.traj_length.length[1]<<8) );
		return 0;
	case ID_MSG_ORDER_TRAJ_SET_NEW_POINT:
		trajAddPoint(traj,
				(uint8_t*)incoming_traj_message.arguments.message_antenne);
		return 0;
	case ID_MSG_ORDER_TRAJ_REINI:
		trajInit(traj);
		return 0;
	case ID_MSG_ORDER_TRAJ_CHECK_CORRECT:
		writeLogToFifo(fifo_log,ID_MSG_LOG_TRAJ_RESPONSE_CORRECT,
				(ARGS){.state=(A_StateArgs){
						 .value=trajCheckCorrectLength(traj)}
					  });
		return 0;
	default:
		writeLogToFifo(fifo_log,ID_MSG_ALERT_BAD_MESSAGE_ID,
				incoming_traj_message.arguments);
		return -1;
	}
}

int handleCommunMessage(objects_fifo_t*  fifo_log,objects_fifo_t* fifo_order,
								SimpleMessage incoming_message){

	switch(incoming_message.id){
	case ID_MSG_ORDER_SURVIE:
		//TODO Handle high priority
		incoming_message.id= ID_MSG_ORDER_ANTENNA;
		strcpy(incoming_message.arguments.message_antenne,ANTENNA_SURVIE);
		break;
	case ID_MSG_ORDER_DESACTIVATE:
		incoming_message.id= ID_MSG_ORDER_ANTENNA;
		strcpy(incoming_message.arguments.message_antenne,ANTENNA_DESACTIVATE);
		break;
	case ID_MSG_ORDER_REINI:
		incoming_message.id= ID_MSG_ORDER_ANTENNA;
		strcpy(incoming_message.arguments.message_antenne,ANTENNA_REINI);
		break;
	case ID_MSG_ORDER_CALAGE:
		incoming_message.id= ID_MSG_ORDER_ANTENNA;
		strcpy(incoming_message.arguments.message_antenne,ANTENNA_CALLAGE);
		break;
	case ID_MSG_ORDER_GOTO:
		//TODO handle ALL commun orders
		break;
	case ID_MSG_ORDER_ANTENNA:
	case ID_MSG_ORDER_DO_TRAJ_AT_DATE:
		break;
	default:
		writeLogToFifo(fifo_log,ID_MSG_ALERT_BAD_MESSAGE_ID,
			incoming_message.arguments);
		return -1;
	}

	//Send to Antenna Executer
	SimpleMessage* new_order=(SimpleMessage*)chFifoTakeObjectI(fifo_order);
	*new_order=incoming_message;
	chFifoSendObjectI(fifo_order,  (void*)new_order);
	return 0;
}
