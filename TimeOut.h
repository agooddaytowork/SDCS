/*
 * TimOut.h
 *
 *  Created on: Aug 19, 2015
 *      Author: tam
 */

#ifndef SDCS_V0_TIMEOUT_TIMEOUT_H_
#define SDCS_V0_TIMEOUT_TIMEOUT_H_
#include "driverlib.h"

void startTimer();
uint32_t timerElapsed();
void registerTimeOut(uint32_t * input);
uint8_t timeIsOut(uint32_t input, uint32_t timeInterval);
uint32_t TITaskDelayUntil(uint32_t wakeTime, uint32_t timeInterval);
void Delay_50ms(uint32_t Interval);
#endif /* SDCS_V0_TIMEOUT_TIMEOUT_H_ */
