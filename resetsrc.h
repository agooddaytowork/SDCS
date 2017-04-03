/*
 * resetsrc.h
 *
 *  Created on: Oct 21, 2015
 *      Author: tam
 */

#ifndef RESETSRC_H_
#define RESETSRC_H_

#include "driverlib.h"
// Hard reset define source
#define RESET_SRC_SYSRESETREQ 						RESET_SRC_0
#define RESET_SRC_WDT_ATIMEOUT 						RESET_SRC_1
#define RESET_SRC_WDT_A_PASSWORDVIOLATION 			RESET_SRC_2
#define RESET_SRC_FLASHCONTROLLER					RESET_SRC_3
#define RESET_SRC_CS								RESET_SRC_14
#define RESET_SRC_PCM								RESET_SRC_15
#define RESET_SRC_FAULTISR							RESET_SRC_5
//Soft reset define source

#define RESET_CPULOCK RESET_SRC_0

#endif /* RESETSRC_H_ */
