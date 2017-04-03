/*
 * wrappedi2c.h
 *
 *  Created on: Jun 30, 2015
 *      Author: Dinh
 */

#ifndef OLEDSAMPLEAPP_WRAPPEDI2C_WRAPPEDI2C_H_
#define OLEDSAMPLEAPP_WRAPPEDI2C_WRAPPEDI2C_H_

#include <stdbool.h>
#include <stdint.h>

#include "driverlib.h"


#define I2C_ADDR 0x3C

void I2C_Init1();
uint8_t sendI2CBuffer();

void addToI2CBuffer(uint8_t c);
void Delay_10us(uint32_t Delay_time);
extern unsigned int i2c_indicator;
extern uint8_t i2c_buffer[1035];


#endif /* OLEDSAMPLEAPP_WRAPPEDI2C_WRAPPEDI2C_H_ */
