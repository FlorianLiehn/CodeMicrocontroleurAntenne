/*
 * PcSerialFunctions.c
 *
 *  Created on: 20 juin 2018
 *      Author: liehnfl
 */

#include "PcSerialThreads.h"

int ManageIncommingMessage(objects_fifo_t*  fifo_log,objects_fifo_t* fifo_order,
						Trajectory* traj,SerialPayload incoming_message){

	if(incoming_message.simple_message.id>=ID_MSG_LOG_ANTENNA_RETURN){

		WriteLogToFifo(fifo_log,ID_MSG_ALERT_BAD_MESSAGE_ID,
			incoming_message.simple_message.arguments);
		return -1;
	}

	//Re-Send order as Log message
	WriteLogToFifo(fifo_log,
			incoming_message.simple_message.id+ID_MSG_LOG_REEMIT_OFFSET,
		incoming_message.simple_message.arguments);

	//Send to Antenna Executer
	SimpleMessage* new_order=(SimpleMessage*)
			chFifoTakeObjectI(fifo_order);
	new_order->arguments=incoming_message.stamp_message.arguments;
	new_order->id=incoming_message.stamp_message.id;
	if( new_order->id==ID_MSG_ORDER_SURVIE){
		new_order->id= ID_MSG_ORDER_ANTENNA;
		strcpy(new_order->arguments.message_antenne,
				ANTENNA_SURVIE);
	}
	chFifoSendObjectI(fifo_order,  (void*)new_order);
	return 0;
}
