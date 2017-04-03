/*
 * GaugeSensor.h
 *
 *  Created on: Jan 6, 2016
 *      Author: TamDuong
 */

#ifndef GAUGESENSOR_GAUGESENSOR_H_
#define GAUGESENSOR_GAUGESENSOR_H_

#include "driverlib.h"
#include "RingBuffer.h"
#include "string.h"
#include "stdlib.h"
#include "../CAN/mcp2515.h"
extern uint16_t GaugePressureValue;
void GaugeSensor_Read_Init(void);
void Gauge_Read_Value(void);
#endif /* GAUGESENSOR_GAUGESENSOR_H_ */
