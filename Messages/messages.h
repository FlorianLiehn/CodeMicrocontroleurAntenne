/*
 * messages.h
 *
 *  Created on: 8 juin 2018
 *      Author: liehnfl
 */

#ifndef MESSAGES_MESSAGES_H_
#define MESSAGES_MESSAGES_H_

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
}goto_args;

//Union of all args for all messages
union ARGS{
	char message_antenne[12];
	char date[12];
	struct goto_args ARGS_goto;
}ARGS;

//////////////////MESSAGES DEFINITION//////////////////

//message for microcontroler
struct order_message{
	uint8_t order;//fill with ORDER enum
	union ARGS arguments;
}order_message;

//log or reply from microcontroler
struct log_message{
	uint8_t order;
	union ARGS logs;
}log_message;

union Payload_message{
	struct log_message 	message;
	char buffer[sizeof(struct log_message)];
}Payload_message;

#endif /* MESSAGES_MESSAGES_H_ */
