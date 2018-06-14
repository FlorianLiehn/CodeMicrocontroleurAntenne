/*
 * AntennaRxThread.c
 *
 *  Created on: 13 juin 2018
 *      Author: liehnfl
 */

#include "AntennaThreads.h"

const SerialConfig AntennaSerialConfig =  {
  19200,
  0,
  USART_CR2_STOP1_BITS | USART_CR2_LINEN,
  0
};
