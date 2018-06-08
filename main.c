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
#include "rt_test_root.h"
#include "oslib_test_root.h"

#include "PC_interface/PcSerialThreads.h"


#define NUM_BUFFERS 20

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

//CONNECTIONS EXTERNES
  /**
   * SD2 = PC
   * A2 et A3
   * **/
  sdStart(&SD2, &PcSerialConfig);
  palSetPadMode(GPIOA, 2, PAL_MODE_ALTERNATE(7));
  palSetPadMode(GPIOA, 3, PAL_MODE_ALTERNATE(7));

//CONNECTIONS INTERNES
  // FIFO that contain all logs and will be send to PC
  msg_t  mailbox_log_buffer[NUM_BUFFERS];
  mailbox_t  mailbox_log;
  chMBObjectInit(&mailbox_log, mailbox_log_buffer, NUM_BUFFERS);
  // FIFO that contain all orders, must be execute in the correct order
  msg_t  mailbox_order_buffer[NUM_BUFFERS];
  mailbox_t  mailbox_order;
  chMBObjectInit(&mailbox_order, mailbox_order_buffer, NUM_BUFFERS);

  /*
   * Creates threads.
   */
  chThdCreateStatic(waPC_RxThread, sizeof(waPC_RxThread), NORMALPRIO, PC_RxThread,
               (void*)&(struct RxThread_args){&mailbox_log  ,&mailbox_order,});
  chThdCreateStatic(waPC_TxThread, sizeof(waPC_TxThread), NORMALPRIO, PC_TxThread,
               	   	   	   	   	   	   	   	   	   	   	   (void*)&mailbox_log);


  while (true) {
    chThdSleepMilliseconds(500);
  }
}
