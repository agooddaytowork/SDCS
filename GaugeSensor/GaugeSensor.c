/*
 * GaugeSensor.c
 *
 *  Created on: Jan 6, 2016
 *      Author: TamDuong
 */

#include "GaugeSensor.h"
#define GAUGERINGLENGTH 100

RingBuffer GagueDataRing;
uint8_t GaugeRingBuffer[GAUGERINGLENGTH];
extern uint16_t GaugePressureValue;
extern volatile uint8_t SDCS_status;
extern uint8_t PresencePacket;

/*
 * UART config: baudrate 9600, LSB, 8bit, NO PARITY, clock SMCLK - 24MHz, over sampling mode
 */
const eUSCI_UART_Config subUartConfig =
{ EUSCI_A_UART_CLOCKSOURCE_SMCLK,          // SMCLK Clock Source
		156,                                     // BRDIV
		4,                                       // UCxBRF
		0,                                       // UCxBRS
		EUSCI_A_UART_NO_PARITY,                  // No Parity
		EUSCI_A_UART_LSB_FIRST,                  // MSB First
		EUSCI_A_UART_ONE_STOP_BIT,               // One stop bit
		EUSCI_A_UART_MODE,                       // UART mode
		EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION  // Oversampling
		};

void GaugeSensor_Read_Init(void)
{
	// Init Ring Buffer
	ringbuffer_init(&GagueDataRing, GaugeRingBuffer, GAUGERINGLENGTH);
	/* Selecting P3.2 and P3.3 in UART mode */
	MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P3,
	GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);

	/* Configuring UART mode for EUSCI_A2_Module */
	MAP_UART_initModule(EUSCI_A2_MODULE, &subUartConfig);
	MAP_UART_enableModule(EUSCI_A2_MODULE);
	MAP_UART_enableInterrupt(EUSCI_A2_MODULE, EUSCI_A_UART_RECEIVE_INTERRUPT);
	//	MAP_Interrupt_enableInterrupt(INT_EUSCIA2);
	/* Enable UART module */

}

void Gauge_Read_Value(void)
{
	uint8_t tmpbyte = 0;
	uint8_t ReadSeq[5];
	double tmpValue = 0;

	ringbuffer_read_byte(&GagueDataRing, &tmpbyte);
	if( tmpbyte == 0x0D)
	{
		ringbuffer_read_history_elements(&GagueDataRing,ReadSeq,5);
		tmpValue = atof((char*) ReadSeq);
		GaugePressureValue = tmpValue*1000;
		ringbuffer_empty(&GagueDataRing);

	}
}
/*
 * GaugeSensor ISR -> RS232
 */
void gaugesensorisr()
{
	uint64_t status;
	uint8_t tmp;
	static uint8_t stage = 0;
	status = UART_getEnabledInterruptStatus(EUSCI_A2_MODULE);
	UART_clearInterruptFlag(EUSCI_A2_MODULE, status);
	if (status & EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG)
	{
		tmp = (uint8_t) MAP_UART_receiveData(EUSCI_A2_MODULE);
		if (tmp != NULL)
		{
			ringbuffer_write_byte(&GagueDataRing, tmp);

			if(stage == 0)
			{
				stage = 1;
				PresencePacket |= CAN_HAS_GAUGE;
			}
		}
	}
}
