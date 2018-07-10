/*
    ChibiOS - Copyright (C) 2006..2018 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include "ch.h"
#include "hal.h"

//Threads definitions
#include "PC_interface/PcSerialThreads.h"
#include "Antenna_interface/AntennaThreads.h"
#include "GpsTimeHandler/GpsTimeHandler.h"
//Trajectory object
#include "Trajectory/Trajectory.h"

//CONNECTIONS INTERNES
// FIFO that contain all logs and will be send to PC
msg_t  msg_log_buffer[FIFO_BUFFER_SIZE];
StampedMessage  logs_buffer[FIFO_BUFFER_SIZE];
objects_fifo_t Fifo_log;
// FIFO that contain all orders, must be execute in the correct order
msg_t  msg_order_buffer[FIFO_BUFFER_SIZE];
SimpleMessage  orders_buffer[FIFO_BUFFER_SIZE];
objects_fifo_t Fifo_order;

//Current traj loaded
Trajectory current_traj;

//pinout userbutton & timeout
#define READ_USER_BUTTON palReadPad(GPIOA,GPIOA_BUTTON)
#define USER_BUTTON_TIMEOUT_MS 150

/*
 * Application entry point.
 */
int main(void) {

  /*
   * System initializations.
   * - HAL initialization, this also initializes the configured device drivers
   *   and performs the board-specific initializations.
   * - Kernel initialization, the main() function becomes a thread and the
   *   RTOS is active.
   */
  halInit();
  chSysInit();

  //Init CRC table
  crcInit();

  //Init Trajectory table
  trajInit(&current_traj);

  //Init Connections internes
  chFifoObjectInit(&Fifo_log,sizeof(StampedMessage),
		  FIFO_BUFFER_SIZE,0,(void*)logs_buffer,msg_log_buffer);

  chFifoObjectInit(&Fifo_order,sizeof(SimpleMessage),
		  FIFO_BUFFER_SIZE,0,(void*)orders_buffer,msg_order_buffer);

  ///////////Enable Threads///////////////
  //enable PC communication & create corresponding Threads
  startPcThreads(&Fifo_log, &Fifo_order,&current_traj);
  //enable Antenna communication
  startAntennaThreads(&Fifo_log, &Fifo_order,&current_traj);
  //enable GPS Time Update
  startGpsThread(&Fifo_log);


  palSetPad(GPIOD, GPIOD_LED6);
  int phase=0;
  while (TRUE) {
		chThdSleepMilliseconds(USER_BUTTON_TIMEOUT_MS);
	if(READ_USER_BUTTON){
		phase=1-phase;
		if(phase){
			palClearPad(GPIOD, GPIOD_LED6);
			palSetPad(GPIOD, GPIOD_LED5);
		}
		else{
			palSetPad(GPIOD, GPIOD_LED6);
			palClearPad(GPIOD, GPIOD_LED5);
		}
		//EMERGENCY USER COMMAND
		SimpleMessage* new_order=(SimpleMessage*)chFifoTakeObjectI(&Fifo_order);
		new_order->id=ID_MSG_ORDER_ANTENNA;
		strncpy(new_order->arguments.message_antenne,ANTENNA_SURVIE,
				ANTENNA_MESSAGE_LENGTH);
		chFifoSendObjectI(&Fifo_order,  (void*)new_order);

	}
  }
}
