/*
 * SDCS.h
 *
 *  Created on: Jan 6, 2016
 *      Author: TamDuong
 */

#ifndef SDCS_H_
#define SDCS_H_


extern volatile uint8_t SDCS_status;
#define SDCS_READ_REQUEST 	0b10000000
#define SDCS_CAN_ENABLE 	0b00000001
#define SDCS_FILTER_ENABLE  0b00000010
#define SDCS_CAN_BUSY		0b00000100
#define SDCS_GAUGE_DETECTED 0b00001000
#define SDCS_FOUND_EDCTU	0b00010000
#define SDCS_WDTA_RESET		0b00100000

#endif /* SDCS_H_ */
