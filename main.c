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

//INTERNALS CONNECTIONS
//To initialize Fifo, it need 2 arrays: msg table & object table
//here the object stored in fifo is StampedMessage object

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

  //Init static CRC table
  crcInit();

  //Init Trajectory table explain!
  trajInit(&current_traj);

  //Init internals connections (FIFOs)
  //init fifo with fifo pointer, size of object, size of fifo,
  // 0 alignment , object table(as pointer) and msg table
  //Log Fifo
  chFifoObjectInit(&Fifo_log,sizeof(StampedMessage),
		  FIFO_BUFFER_SIZE,0,(void*)logs_buffer,msg_log_buffer);
  //order Fifo
  chFifoObjectInit(&Fifo_order,sizeof(SimpleMessage),
		  FIFO_BUFFER_SIZE,0,(void*)orders_buffer,msg_order_buffer);

  ///////////Enable Threads///////////////
  //Each thread need to be connected to different objects
  //They all used log fifo pointer for example

  //enable PC communication & create corresponding Threads
  startPcThreads(&Fifo_log, &Fifo_order,&current_traj);
  //enable Antenna communication
  startAntennaThreads(&Fifo_log, &Fifo_order,&current_traj);
  //enable GPS Time Update
  startGpsThread(&Fifo_log);

  ///////////main program///////////////
  //will handle User Button(Blue)

  //Init Led 1 On Led 2 Off
  palSetPad(GPIOD, GPIOD_LED6);
  palClearPad(GPIOD, GPIOD_LED5);
  int phase=0;
  //infinite loop
  while (TRUE) {
		chThdSleepMilliseconds(USER_BUTTON_TIMEOUT_MS);
	if(READ_USER_BUTTON){
		phase=1-phase;
		if(phase){
			//Led 1 Off Led 2 On
			palClearPad(GPIOD, GPIOD_LED6);
			palSetPad(GPIOD, GPIOD_LED5);
		}
		else{
			//Led 1 On  Led 2 Off
			palSetPad(GPIOD, GPIOD_LED6);
			palClearPad(GPIOD, GPIOD_LED5);
		}
		//EMERGENCY USER COMMAND
		//create a emergency message
		SimpleMessage* new_order=(SimpleMessage*)
				chFifoTakeObjectTimeoutS(&Fifo_order,TIME_IMMEDIATE);
		new_order->id=ID_MSG_ORDER_ANTENNA;
		strncpy(new_order->arguments.message_antenne,ANTENNA_SURVIE,
				ANTENNA_MESSAGE_LENGTH);
		//send the message with order fifo
		chFifoSendObjectS(&Fifo_order,  (void*)new_order);

	}
  }
}
