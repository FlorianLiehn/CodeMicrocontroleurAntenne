/*
 * PcSerialThreads.c
 *
 *  Created on: 8 juin 2018
 *      Author: liehnfl
 */

#include "PcSerialThreads.h"

const SerialConfig PcSerialConfig =  {
  115200,
  0,
  USART_CR2_STOP1_BITS /*| USART_CR2_LINEN*/,
  0
};

THD_WORKING_AREA(waPC_TxThread, 128);
THD_FUNCTION(PC_TxThread, arg) {

	objects_fifo_t*  fifo_log_arg  =(objects_fifo_t*)arg;
	void* msg;
	uint8_t emit_buffer[serialMessageLength];

	chRegSetThreadName("Thread TX PC");

	int phase=0;
	palSetPad(GPIOD, GPIOD_LED6);
	while (true) {

		msg_t state = chFifoReceiveObjectI(fifo_log_arg,&msg);

		if(state==MSG_OK){
			//send message
			encodePayload((char*)msg,emit_buffer);

			//sdWrite(&SD2, emit_buffer,serialMessageLength);
			sdAsynchronousWrite(&SD2, emit_buffer,serialMessageLength);

			phase=1-phase;
			if(phase){
				palClearPad(GPIOD, GPIOD_LED6);
				palSetPad(GPIOD, GPIOD_LED5);
			}
			else{
				palSetPad(GPIOD, GPIOD_LED6);
				palClearPad(GPIOD, GPIOD_LED5);
			}
		}
		chThdSleepMilliseconds(2);
	}
}

THD_WORKING_AREA(waPC_RxThread, 128);
THD_FUNCTION(PC_RxThread, arg) {

	objects_fifo_t*  fifo_log_arg  =((struct RxThread_args*)arg)->fifo_log_arg;
	//objects_fifo_t*  fifo_order_arg=((struct RxThread_args*)arg)->fifo_order_arg;
	union Payload_message in_message;

	chRegSetThreadName("Thread RX PC");

	int phase=0;
	int count=0;
	palSetPad(GPIOD, GPIOD_LED3);
	while (true) {
		count++;
		int status=read_message((uint8_t*)&in_message.buffer);

		if(status==0){
			//TODO Log CRC problem
		}
		else if(status>0){
			phase=1-phase;

			struct log_message* new_message=next_message();
			*new_message=in_message.message;
			chFifoSendObjectI(fifo_log_arg,  (void*)new_message);
		}


		if(phase){
			palClearPad(GPIOD, GPIOD_LED3);
			palSetPad(GPIOD, GPIOD_LED4);
		}
		else{
			palSetPad(GPIOD, GPIOD_LED3);
			palClearPad(GPIOD, GPIOD_LED4);
		}

		if(count%20==0){
			struct log_message* new_message=next_message();
			new_message->order=ORDER_GOTO;
			strncpy(new_message->logs.message_antenne,"TEST0MESSAGE",12);
			new_message->logs.message_antenne[4]+=(count/20)%10;
			chFifoSendObjectI(fifo_log_arg,(void*)new_message);
		}

		chThdSleepMilliseconds(25);

	}
}

int read_message(uint8_t* message){

	uint8_t buf [serialMessageLength];
    memset (message, 0, Payload_message_lenght);
    //Read Header Byte
	int n=sdAsynchronousRead(&SD2,buf,1);

	if(n!=1 || buf[0]!=HEADER_BYTE)
		return -1;
	//Read Payload lenght
	while(n==1)
		n+=sdAsynchronousRead(&SD2,&(buf[n]),1);//read nb
	int tot=(int)(buf[1]);
	//Read Payload + CRC
	while(n<tot+3){
		n+=sdAsynchronousRead(&SD2,&(buf[n]),tot+3-n);
	}
	strncpy((char*)message,(char*)&(buf[2]),tot);
	//Check CRC
	uint8_t crc=ComputeCRC(message,tot);
	if(crc!=buf[tot+3-1]){
		return 0;
	}
	return 1;
}


static struct log_message messages_buffer[FIFO_BUFFER_SIZE];
static int log_pointer=0;
struct log_message* next_message(){
	log_pointer++;
	if(log_pointer>=FIFO_BUFFER_SIZE)
		log_pointer=0;
	return &messages_buffer[log_pointer];

}

