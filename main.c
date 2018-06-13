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

#include "PC_interface/PcSerialThreads.h"

//CONNECTIONS INTERNES
// FIFO that contain all logs and will be send to PC
msg_t  msg_log_buffer[FIFO_BUFFER_SIZE];
struct log_message  logs_buffer[FIFO_BUFFER_SIZE];
objects_fifo_t Fifo_log;
// FIFO that contain all orders, must be execute in the correct order
msg_t  msg_order_buffer[FIFO_BUFFER_SIZE];
struct log_message  orders_buffer[FIFO_BUFFER_SIZE];
objects_fifo_t Fifo_order;

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

  //Init Connections internes
  chFifoObjectInit(&Fifo_log,sizeof(struct log_message),
		  FIFO_BUFFER_SIZE,0,(void*)logs_buffer,msg_log_buffer);

  chFifoObjectInit(&Fifo_order,sizeof(struct log_message),
		  FIFO_BUFFER_SIZE,0,(void*)orders_buffer,msg_order_buffer);
//CONNECTIONS EXTERNES
  /**
   * SD2 = PC
   * A2 et A3
   * **/
  sdStart(&SD2, &PcSerialConfig);
  palSetPadMode(GPIOA, 2, PAL_MODE_ALTERNATE(7));
  palSetPadMode(GPIOA, 3, PAL_MODE_ALTERNATE(7));

  /*
   * Creates threads.
   */
  chThdCreateStatic(waPC_RxThread, sizeof(waPC_RxThread), NORMALPRIO, PC_RxThread,
               (void*)&(struct RxThread_args){&Fifo_log  ,&Fifo_order,});
  chThdCreateStatic(waPC_TxThread, sizeof(waPC_TxThread), NORMALPRIO, PC_TxThread,
               	   	   	   	   	   	   	   	   	   	   	   (void*)&Fifo_log);


  while (true) {
    chThdSleepMilliseconds(500);
  }
}
