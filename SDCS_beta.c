#include "driverlib.h"
#include "string.h"
#include "OLEDlibrary/OLED.h"
#include "RFID/RC522.h"
#include "TimeOut.h"
#include "GaugeSensor/GaugeSensor.h"
#include "SDCS.h"
#include "CAN/mcp2515.h"
#include "printf.h"
#include "resetsrc.h"

#define ADCBUFFERLENGTH 32 // 32 samples
#define BUFFER_FULL 1
#define BUFFER_NOT_FULL 0

#define ADCLOWERVAL 800 // Bottom pressure point 2.0*E^-11
#define ADCHIGHERVAL 13000 // Upper pressure point 5.7*E^-7
//uint8_t reset_cause_global = 0;
// Init GLOBAL FLAG

volatile uint8_t SDCS_status = 0;
//extern uint8_t OLED_display_array[3][7];
/*
 *  Init GlOBAL DATA
 */

volatile uint8_t SDCS_NW_ID = 2; // Default SDCS Network ID
uint16_t GaugePressureValue = 0;
uint8_t Inputstate[6];
// Init ADC Variable
uint16_t ADCSeq[6];
uint32_t ADCsum[6];
uint16_t ADCresult[6];

/****
 *
 */

uint8_t IdleMode = 0;
/*
 *				 Data Packet
 *				 7 packets, 8 byte each
 *	1st 6 packets:			 1st 4 bytes : UUID, 2nd 2 bytes Pressure Value, 3rd 2 bytes pressure value
 *	2nd 1 packet:  1st 2 bytes: Gauge Value, 2nd 6 bytes SDCS status
 *
 */
uint8_t DaTaPacket[7][8];

/*
 * Presence Packet
 *
 */

uint8_t PresencePacket = 0;
// RFID UUID

uint8_t UUID[6][5];

void getRS_src() {
	uint32_t reset_cause;
	reset_cause = ResetCtl_getHardResetSource();
	ResetCtl_clearHardResetSource(reset_cause);
	switch (reset_cause) {
	case RESET_SRC_CS: // occur when hitting reset button
		//	setBit(&reset_cause_global, RESET_CAUSE1);

		break;
	case RESET_SRC_PCM:
		//setBit(&reset_cause_global, RESET_CAUSE2);
		break;
	case RESET_SRC_SYSRESETREQ:
		//setBit(&reset_cause_global, RESET_CAUSE3);
		SDCS_status |= SDCS_WDTA_RESET;
		break;
	case RESET_SRC_FLASHCONTROLLER:
		//	setBit(&reset_cause_global, RESET_CAUSE4);
		SDCS_status |= SDCS_WDTA_RESET;
		break;
	case RESET_SRC_WDT_ATIMEOUT:
		//	setBit(&reset_cause_global, RESET_CAUSE5);
		SDCS_status |= SDCS_WDTA_RESET;
		break;
	case RESET_SRC_FAULTISR:
		//	setBit(&reset_cause_global, RESET_CAUSE6);
		SDCS_status |= SDCS_WDTA_RESET;

		break;

	}
	reset_cause = ResetCtl_getSoftResetSource();
	ResetCtl_clearSoftResetSource(reset_cause);
}

void Watchdog_Init(void) {
	// ACLK = 16KHz, Iteration cycles = 512K, Iteration time = 32s
	MAP_WDT_A_initWatchdogTimer(WDT_A_CLOCKSOURCE_ACLK,
	WDT_A_CLOCKITERATIONS_512K);
	MAP_SysCtl_setWDTTimeoutResetType(SYSCTL_HARD_RESET);

}
void MainClock_Init()

{
	WDT_A_holdTimer();
	MAP_PCM_setCoreVoltageLevel(PCM_VCORE1);
	MAP_FlashCtl_setWaitState(FLASH_BANK0, 1);
	MAP_FlashCtl_setWaitState(FLASH_BANK1, 1);

	MAP_CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_48);
	// set SMCLK source = DCO, scale 2 = 24MHz
	MAP_CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_2);
	// set MCLK source = DCO, scale 2 = 3MHz
	MAP_CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_16);
	// set ACLK = 16KHz
	MAP_CS_initClockSignal(CS_ACLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_2);
	__delay_cycles(2000000);
}

const eUSCI_UART_Config uartConfig = {
EUSCI_A_UART_CLOCKSOURCE_SMCLK,          // SMCLK Clock Source
		13,                                     // BRDIV = 78
		0,                                       // UCxBRF = 2
		37,                                       // UCxBRS = 0
		EUSCI_A_UART_NO_PARITY,                  // No Parity
		EUSCI_A_UART_LSB_FIRST,                  // MSB First
		EUSCI_A_UART_ONE_STOP_BIT,               // One stop bit
		EUSCI_A_UART_MODE,                       // UART mode
		EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION  // Oversampling
		};
void UART_init() {
	/* Selecting P1.2 and P1.3 in UART mode */
	MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,
	GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);

	/* Configuring UART Module */
	MAP_UART_initModule(EUSCI_A0_MODULE, &uartConfig);

	/* Enable UART module */
	MAP_UART_enableModule(EUSCI_A0_MODULE);
}

// 50ms
const Timer_A_UpModeConfig upConfig = {
TIMER_A_CLOCKSOURCE_ACLK,              // ACLK Clock Source = 16KHz
		TIMER_A_CLOCKSOURCE_DIVIDER_16,          // ACLK/16 = 1KHz
		10,                           // 50 tick period
		TIMER_A_TAIE_INTERRUPT_DISABLE,         // Disable Timer interrupt
		TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE,    // Enable CCR0 interrupt
		TIMER_A_DO_CLEAR                        // Clear value
		};

void SystemTick_Init() {
	MAP_SysTick_enableModule();
	MAP_SysTick_setPeriod(480000); // 50ms base
	MAP_SysTick_enableInterrupt();
}

void ADC_Init() {
	// set 2.5V REF
	MAP_REF_A_enableReferenceVoltage();
	MAP_REF_A_setReferenceVoltage(REF_A_VREF2_5V);
	//MAP_REF_A_disableReferenceVoltageOutput();
	MAP_REF_A_enableReferenceVoltageOutput();
	/* Initializing ADC SMCLK  */
	/* Setting reference voltage to AVCC  and enabling reference */

	MAP_ADC14_enableModule();
	MAP_ADC14_initModule(ADC_CLOCKSOURCE_SMCLK, ADC_PREDIVIDER_1, ADC_DIVIDER_1,
			0);
	MAP_ADC14_setPowerMode(ADC_UNRESTRICTED_POWER_MODE);

	/* Configuring GPIOs (9.1 A16) O1 */
	MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P9, GPIO_PIN1,
	GPIO_TERTIARY_MODULE_FUNCTION);
	/* Configuring GPIOs (9.0 A17) O2*/
	MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P9, GPIO_PIN0,
	GPIO_TERTIARY_MODULE_FUNCTION);
	/* Configuring GPIOs (8.7 A18) O3*/
	MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P8, GPIO_PIN7,
	GPIO_TERTIARY_MODULE_FUNCTION);
	/* Configuring GPIOs (8.6 A19) O4*/
	MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P8, GPIO_PIN6,
	GPIO_TERTIARY_MODULE_FUNCTION);
	/* Configuring GPIOs (8.5 A20) O5*/
	MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P8, GPIO_PIN5,
	GPIO_TERTIARY_MODULE_FUNCTION);
	/* Configuring GPIOs (8.4 A21) O6*/
	MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P8, GPIO_PIN4,
	GPIO_TERTIARY_MODULE_FUNCTION);
	/* Configuring GPIOs (8.3 A22) O7
	 MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P8, GPIO_PIN3,
	 GPIO_TERTIARY_MODULE_FUNCTION);*/

	/* Configuring ADC Memory */
	//MAP_ADC14_configureSingleSampleMode(ADC_MEM0, true);
	MAP_ADC14_configureMultiSequenceMode(ADC_MEM16, ADC_MEM21, false);
	MAP_ADC14_configureConversionMemory(ADC_MEM16,
	ADC_VREFPOS_INTBUF_VREFNEG_VSS,
	ADC_INPUT_A16, false);
	MAP_ADC14_configureConversionMemory(ADC_MEM17,
	ADC_VREFPOS_INTBUF_VREFNEG_VSS,
	ADC_INPUT_A17, false);
	MAP_ADC14_configureConversionMemory(ADC_MEM18,
	ADC_VREFPOS_INTBUF_VREFNEG_VSS,
	ADC_INPUT_A18, false);
	MAP_ADC14_configureConversionMemory(ADC_MEM19,
	ADC_VREFPOS_INTBUF_VREFNEG_VSS,
	ADC_INPUT_A19, false);
	MAP_ADC14_configureConversionMemory(ADC_MEM20,
	ADC_VREFPOS_INTBUF_VREFNEG_VSS,
	ADC_INPUT_A20, false);
	MAP_ADC14_configureConversionMemory(ADC_MEM21,
	ADC_VREFPOS_INTBUF_VREFNEG_VSS,
	ADC_INPUT_A21, false);
	/*MAP_ADC14_configureConversionMemory(ADC_MEM22,
	 ADC_VREFPOS_INTBUF_VREFNEG_VSS,
	 ADC_INPUT_A22, false);*/

	/* Configuring Sample Timer */
	MAP_ADC14_enableSampleTimer(ADC_AUTOMATIC_ITERATION);
	MAP_ADC14_setResolution(ADC_14BIT);
	/* Enabling interrupts */
	MAP_ADC14_enableInterrupt(ADC_INT21);
	MAP_Interrupt_enableInterrupt(INT_ADC14);

	/* Enabling/Toggling Conversion */
	MAP_ADC14_enableConversion();
	MAP_ADC14_toggleConversionTrigger();

}

void InputSwitches_Init() {
	GPIO_setAsOutputPin(GPIO_PORT_P10,
	GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 | GPIO_PIN4 | GPIO_PIN5);
	GPIO_setOutputHighOnPin(GPIO_PORT_P10,
	GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 | GPIO_PIN4 | GPIO_PIN5);
	//GPIO_setAsOutputPin(GPIO_PORT_P10, GPIO_PIN3);
	//GPIO_setOutputHighOnPin(GPIO_PORT_P10, GPIO_PIN3);
}

/*
 * Button Init
 */
void Button_Init() {
	MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P5, GPIO_PIN0);
	MAP_GPIO_interruptEdgeSelect(GPIO_PORT_P5, GPIO_PIN0,
	GPIO_HIGH_TO_LOW_TRANSITION);
	MAP_GPIO_enableInterrupt(GPIO_PORT_P5, GPIO_PIN0);
	MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P5, GPIO_PIN2);
	MAP_GPIO_interruptEdgeSelect(GPIO_PORT_P5, GPIO_PIN2,
	GPIO_HIGH_TO_LOW_TRANSITION);
	MAP_GPIO_enableInterrupt(GPIO_PORT_P5, GPIO_PIN2);

	MAP_Interrupt_enableInterrupt(INT_PORT5);
	MAP_Interrupt_enableInterrupt(INT_PORT2);
	// Green LED P2.1 Init
	GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN1);
	GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1);
	// Init P6.0 For OLED reset Pin

	GPIO_setAsOutputPin(GPIO_PORT_P6, GPIO_PIN0);
	GPIO_setOutputHighOnPin(GPIO_PORT_P6, GPIO_PIN0);
}

void testCAN(void) {
	can_speed(500000, 1, 3);
	can_rx_setmask(0, 0x000000FF, 0);
	can_rx_setmask(1, 0xFF, 0);
	can_rx_setfilter(0, 0, 0xF0);
	can_rx_setfilter(1, 0, 255);
	can_rx_setfilter(1, 1, 255);
	can_rx_setfilter(1, 2, 255);
	can_rx_setfilter(1, 3, 255);
	//can_rx_setfilter(0, 1, ((SDCS_NW_ID - 1) << 4 | 7));
	can_ioctl(MCP2515_OPTION_LOOPBACK, 0);
	can_ioctl(MCP2515_OPTION_ONESHOT, 0);
	//SDCS_status |= SDCS_CAN_ENABLE;
	//PresencePacket |= CAN_CONFIG_VALID |6 | CAN_HAS_GAUGE;
	PresencePacket |= CAN_CONFIG_VALID | 6;
	//PresencePacket |= CAN_HAS_GAUGE;
}
void System_setup() {


	MainClock_Init();
	Watchdog_Init();
	uint8_t i = 0;
	memset(DaTaPacket, 0, 56); // Init DATAPacket
	memset(Inputstate, 0, 6);
//	UART_init(); // Debugging UART
//	SystemTick_Init();
	InputSwitches_Init();
	Button_Init();
	can_spi_init();
	can_init();
	ADC_Init();
	RC522_Init();
	GaugeSensor_Read_Init();
	OLEDbegin();
	GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
	// Set priority
	GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);
	GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);

	/* Configuring Timer_A1 for Up Mode */
	MAP_Timer_A_configureUpMode(TIMER_A1_MODULE, &upConfig);

	/* Enabling interrupts and starting the timer */
	MAP_Interrupt_enableInterrupt(INT_TA1_0);
	MAP_Interrupt_enableInterrupt(INT_EUSCIA2);
	Interrupt_enableInterrupt(INT_PORT2);
	MAP_Interrupt_setPriority(INT_ADC14, 4 << 5); // ADC14
	//MAP_Interrupt_setPriority(15, 2<<5); // System Tick
	MAP_Interrupt_setPriority(INT_TA1_0, 3 << 5); // TA1_0 interrupt
	MAP_Interrupt_setPriority(INT_EUSCIA2, 1 << 5); // GaugeSensor
	MAP_Interrupt_setPriority(INT_PORT2, 2 << 5); // CAN interrupt
	MAP_Interrupt_setPriority(INT_PORT5, 5 << 5); // Button interrupt

	MAP_Interrupt_enableMaster();
	// enable Timer A
	MAP_Timer_A_startCounter(TIMER_A1_MODULE, TIMER_A_UP_MODE);


}

// TEST RFID
void RFID_task(void) {

	uint8_t i = 0;
	static uint16_t counter[6];rf
	for (i = 0; i < 6; i++) {

		// HW reset the RFID reader
		RC522_NRSTPD_LOW();
		RC522_NRSTPD_HIGH();
		if (RFID_ReadTag((i), (uint8_t *) &UUID[i][0])) {
			OLED_Station_eGunisOn_display((i), 1);
			counter[i] = 0;
		} else {
			counter[i]++;
			if (counter[i] == 3) {
				OLED_Station_eGunisOn_display((i), 0);
				UUID[i][0] = 0;
				UUID[i][1] = 0;
				UUID[i][2] = 0;
				UUID[i][3] = 0;
				UUID[i][4] = 0;

				counter[i] = 0;
			}

		}

	}

}

/*
 * Desc: update new data to packet
 */
void Packet_Feeding(void) {
	uint8_t i = 0;
	// inject ADC values and UUID
	for (i = 0; i < 6; i++) {

		DaTaPacket[i][0] = UUID[i][0];
		DaTaPacket[i][1] = UUID[i][1];
		DaTaPacket[i][2] = UUID[i][2];
		DaTaPacket[i][3] = UUID[i][3];
		DaTaPacket[i][4] = (uint8_t) (ADCresult[i] >> 8);
		DaTaPacket[i][5] = (uint8_t) (ADCresult[i] & 0x00ff);
	}

	DaTaPacket[6][0] = (uint8_t) (GaugePressureValue >> 8);
	DaTaPacket[6][1] = (uint8_t) (GaugePressureValue & 0x00ff);

}

/*
 * Desc: check Analog input
 */
void Inputcheck(void) {
	uint8_t cnt = 0;

	for (cnt = 0; cnt < 6; cnt++) {

		if (ADCSeq[cnt] < ADCLOWERVAL || ADCSeq[cnt] > ADCHIGHERVAL) {
			if (Inputstate[cnt] == 1) {
				Inputstate[cnt] = 0;
				OLED_Station_isConnected_display(cnt, 0);
			}
		} else {
			OLED_Station_isConnected_display(cnt, 1);
			Inputstate[cnt] = 1;
		}
	}
}
int main(void)

{
	//uint32_t count = 0;
	uint32_t OLEDResetTime;
	uint32_t OLEDheartBeat = 0;
	uint8_t beatUp = 0;
	uint8_t beat = 0;
	System_setup();
	GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);
	getRS_src();
	testCAN();
	OLED_Menu();
	//printf1(EUSCI_A0_MODULE,"Hello, start\r\n");

	MAP_WDT_A_startTimer();
	registerTimeOut(&OLEDResetTime);
	registerTimeOut(&OLEDheartBeat);
	while (1) {
		MAP_WDT_A_clearTimer();
//		beat =0;
//					while (beat == 0)
//					{
//						if (beatUp == 0)
//						{
//							setCursor(120, 3);
//							OLEDprint("*");
//							beatUp = 1;
//							beat =1;
//							break;
//						}
//						if (beatUp == 1)
//						{
//							setCursor(120, 3);
//							OLEDprint(".");
//							beatUp = 0;
//							beat = 1;
//							break;
//						}
//					}

		RFID_task();
		Gauge_Read_Value();
		Packet_Feeding();
		Inputcheck();
		//printf1(EUSCI_A0_MODULE,"ADC seq 1: %i, cnt: %i\r\n", ADCSeq[0], count++);
		if (timeIsOut(OLEDResetTime, 6000)) {
			registerTimeOut(&OLEDResetTime);
//			OLEDbegin();
//			OLED_Start_Page();
//			setCursor(84, 2);
//			OLEDprint(OLED_display_array[2]);
//			setCursor(84, 0);
//		OLEDprint(OLED_display_array[0]);
			OLEDclear(0, 0, 128, 32);
			IdleMode = 1;

		}

	}
}

void PullDown_Switch(uint8_t channel, uint8_t status) {
	if (status) {
		switch (channel) {
		case 0:
			MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P10, GPIO_PIN0);
			break;
		case 1:
			MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P10, GPIO_PIN1);
			break;
		case 2:
			MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P10, GPIO_PIN2);
			break;
		case 3:
			MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P10, GPIO_PIN3);
			break;
		case 4:
			MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P10, GPIO_PIN4);
			break;
		case 5:
			MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P10, GPIO_PIN5);
			break;

		}
	} else {
		switch (channel) {
		case 0:
			MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P10, GPIO_PIN0);
			break;
		case 1:
			MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P10, GPIO_PIN1);
			break;
		case 2:
			MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P10, GPIO_PIN2);
			break;
		case 3:
			MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P10, GPIO_PIN3);
			break;
		case 4:
			MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P10, GPIO_PIN4);
			break;
		case 5:
			MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P10, GPIO_PIN5);
			break;

		}
	}

}

void adc14isr() {
	uint64_t status;
	uint8_t i = 0;
	static uint8_t ii = 0;
	status = MAP_ADC14_getEnabledInterruptStatus();
	MAP_ADC14_clearInterruptFlag(status);
	if (status & ADC_INT21) {
		MAP_ADC14_getMultiSequenceResult(ADCSeq); // get ADC data from Memory to ADCSeq Array
		for (i = 0; i < 6; i++) {
			ADCsum[i] += ADCSeq[i]; // Assgin ADC data to Sum Array
		}
		ii++; // increase Buffer counter

		if (ii == ADCBUFFERLENGTH) {

			for (i = 0; i < 6; i++) {
				ADCresult[i] = ADCsum[i] / 32; // divide the Sum Array to get AVg value
				if ((ADCresult[i] < ADCLOWERVAL)
						|| (ADCresult[i] > ADCHIGHERVAL)) {
					PullDown_Switch(i, 1);
				} else {
					PullDown_Switch(i, 0);
				}

				ADCsum[i] = 0;
			}
			ii = 0; // Reset Buffer Counter

		}
	}
}

