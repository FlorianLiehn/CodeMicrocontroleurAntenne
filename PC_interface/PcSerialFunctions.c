/*
 * PcSerialFunctions.c
 *
 *  Created on: 20 juin 2018
 *      Author: liehnfl
 */

#include "PcSerialThreads.h"

int HandleIncommingMessage(objects_fifo_t*  fifo_log,objects_fifo_t* fifo_order,
						Trajectory* traj,SimpleMessage incoming_message){

	//Wrong ID ( error or log )
	if(incoming_message.id>=FIRST_ERROR_ID){

		WriteLogToFifo(fifo_log,ID_MSG_ALERT_BAD_MESSAGE_ID,
			incoming_message.arguments);
		return -1;
	}

#ifdef ECHO_COMMAND
	//Re-Send order as Log message
	WriteLogToFifo(fifo_log,
			incoming_message.id+ID_MSG_LOG_REEMIT_OFFSET,
		incoming_message.arguments);
#endif

	//Handle Trajectory
	if(incoming_message.id>=FIRST_ORDER_TRAJ_ID){
		return  HandleTrajectory(fifo_log,traj,incoming_message);
	}

	return HandleCommunMessage(fifo_log,fifo_order,incoming_message);
}


int HandleTrajectory(objects_fifo_t*  fifo_log,
			Trajectory* traj,SimpleMessage incoming_traj_message){

	//TODO handle lock when On

	switch(incoming_traj_message.id){
	case ID_MSG_ORDER_TRAJ_SET_LENGTH:
		TrajSetLength(traj,
				incoming_traj_message.arguments.traj_length.length[0]+
			   (incoming_traj_message.arguments.traj_length.length[1]<<8) );
		return 0;
	case ID_MSG_ORDER_TRAJ_SET_NEW_POINT:
		TrajAddPoint(traj,
				(uint8_t*)incoming_traj_message.arguments.message_antenne);
		return 0;
	case ID_MSG_ORDER_TRAJ_REINI:
		TrajInit(traj);
		return 0;
	case ID_MSG_ORDER_TRAJ_CHECK_CORRECT:
		WriteLogToFifo(fifo_log,ID_MSG_LOG_TRAJ_RESPONSE_CORRECT,
				(ARGS){.state=(A_State_args){
						 .value=CheckTrajCorrectLength(traj)}
					  });
		return 0;
	default:
		WriteLogToFifo(fifo_log,ID_MSG_ALERT_BAD_MESSAGE_ID,
				incoming_traj_message.arguments);
		return -1;
	}
}

int HandleCommunMessage(objects_fifo_t*  fifo_log,objects_fifo_t* fifo_order,
								SimpleMessage incoming_message){

	switch(incoming_message.id){
	case ID_MSG_ORDER_SURVIE:
		//TODO Handle high priority
		incoming_message.id= ID_MSG_ORDER_ANTENNA;
		strcpy(incoming_message.arguments.message_antenne,ANTENNA_SURVIE);
		break;
	case ID_MSG_ORDER_REINI:
		incoming_message.id= ID_MSG_ORDER_ANTENNA;
		strcpy(incoming_message.arguments.message_antenne,ANTENNA_DESACTIVATE);
		break;
	case ID_MSG_ORDER_CALAGE:
		incoming_message.id= ID_MSG_ORDER_ANTENNA;
		strcpy(incoming_message.arguments.message_antenne,ANTENNA_CALLAGE);
		break;
	case ID_MSG_ORDER_GOTO:
		//TODO handle ALL commun orders
		break;
	case ID_MSG_ORDER_ANTENNA:
		break;
	default:
		WriteLogToFifo(fifo_log,ID_MSG_ALERT_BAD_MESSAGE_ID,
			incoming_message.arguments);
		return -1;
	}

	//Send to Antenna Executer
	SimpleMessage* new_order=(SimpleMessage*)chFifoTakeObjectI(fifo_order);
	new_order->arguments=incoming_message.arguments;
	new_order->id=incoming_message.id;

	chFifoSendObjectI(fifo_order,  (void*)new_order);
	return 0;
}
