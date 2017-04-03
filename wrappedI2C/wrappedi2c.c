/*
 * wrappedi2c.c
 *
 *  Created on: Jun 30, 2015
 *      Author: Dinh
 */

#include "wrappedi2c.h"
#include "driverlib.h"
unsigned int i2c_indicator = 0;
uint8_t i2c_buffer[1035];

const eUSCI_I2C_MasterConfig i2cConfig =
{
EUSCI_B_I2C_CLOCKSOURCE_SMCLK,          // SMCLK Clock Source
		24000000,                                // MCLK = 24MHz
		EUSCI_B_I2C_SET_DATA_RATE_400KBPS,      // Desired I2C Clock of 400khz
		//EUSCI_B_I2C_SET_DATA_RATE_100KBPS,	// Desired I2C Clock of 100khz
		0,                                      // No byte counter threshold
		EUSCI_B_I2C_NO_AUTO_STOP                // No Autostop
		};

void I2C_Init1()
{
	/* Select Port 1 for I2C - Set Pin 6, 7 to input Primary Module Function,
	 *   (UCB0SIMO/UCB0SDA, UCB0SOMI/UCB0SCL).
	 */
	MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6,
	GPIO_PIN4 + GPIO_PIN5, GPIO_PRIMARY_MODULE_FUNCTION);
	/* Initializing I2C Master to SMCLK at 400kbs with no autostop */
	MAP_I2C_initMaster(EUSCI_B1_MODULE, &i2cConfig);
	/* Specify slave address */
	MAP_I2C_setSlaveAddress(EUSCI_B1_MODULE, I2C_ADDR);
	/* Set Master in receive mode */
	MAP_I2C_setMode(EUSCI_B1_MODULE, EUSCI_B_I2C_TRANSMIT_MODE);
	/* Enable I2C Module to start operations */
	MAP_I2C_enableModule(EUSCI_B1_MODULE);
	__delay_cycles(200000);
	//Enable master Receive interrupt
	MAP_I2C_enableInterrupt(EUSCI_B1_MODULE,
	EUSCI_B_I2C_TRANSMIT_INTERRUPT0 + EUSCI_B_I2C_NAK_INTERRUPT);
	//MAP_Interrupt_enableInterrupt(INT_EUSCIB0);
	__delay_cycles(200000);
}

void addToI2CBuffer(uint8_t c)
{
	i2c_buffer[i2c_indicator] = c;
	i2c_indicator++;
}

uint8_t sendI2CBuffer()
{

	unsigned int counter = 0;
	if (i2c_indicator > 0)
	{
		if (i2c_indicator > 1)
		{
			while ((I2C_isBusBusy(EUSCI_B1_MODULE)) == EUSCI_B_I2C_BUS_BUSY);
			I2C_masterSendStart(EUSCI_B1_MODULE);

			//__delay_cycles(600);
			counter = 1;
			while (counter <= i2c_indicator)
			{
				while ((I2C_isBusBusy(EUSCI_B1_MODULE)) == EUSCI_B_I2C_BUS_BUSY);
				I2C_masterSendMultiByteNext(EUSCI_B1_MODULE,
						i2c_buffer[counter - 1]);



				//__delay_cycles(600);
				counter++;
			}
			while ((I2C_isBusBusy(EUSCI_B1_MODULE)) == EUSCI_B_I2C_BUS_BUSY);
			I2C_masterSendMultiByteStop(EUSCI_B1_MODULE);


			//__delay_cycles(600);
			i2c_indicator = 0;

			return 1;
		}
		else
		{
			while ((I2C_isBusBusy(EUSCI_B1_MODULE)) == EUSCI_B_I2C_BUS_BUSY);
			I2C_masterSendSingleByte(EUSCI_B1_MODULE, i2c_buffer[0]);
			//__delay_cycles(500);
			i2c_indicator = 0;

			return 2;
		}
	}
	else

		return 0;
}

