/*
 * GUI.c
 *
 *  Created on: Jan 5, 2016
 *      Author: TamDuong
 */
#include "OLEDlibrary/OLED.h"
#include "TimeOut.h"
#include "string.h"
#include "SDCS.h"
#include "CAN/mcp2515.h"

#define Left_Button 0x01
#define RIGHTBUTTON 0x01
#define BUTTONPRESS 0x02
uint8_t Button_Status = 0;
uint8_t SetupFinish = 0;

extern volatile uint8_t SDCS_NW_ID;
extern uint8_t OLED_display_array[3][7];
extern uint8_t IdleMode;
void buttonisr()
{
	static uint32_t counter = 0;
	uint64_t status;
	status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P5);
	GPIO_clearInterruptFlag(GPIO_PORT_P5, status);
	if (timeIsOut(counter, 20))
	{
		registerTimeOut(&counter);

		if (status & GPIO_PIN0)
		{
			Button_Status &= ~(Left_Button); // 00
			//	OLEDclear(0, 0, 128, 64);
			//OLEDprintln("Left");
		}

		if (status & GPIO_PIN2)
		{
			Button_Status |= RIGHTBUTTON; //01
			//	OLEDclear(0, 0, 128, 64);
			//	OLEDprintln("Right");
		}
		GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
		Button_Status |= BUTTONPRESS;

	}
	if (SetupFinish == 1)
	{
		OLED_Start_Page();
		setCursor(84, 2);
		OLEDprint(OLED_display_array[2]);
		setCursor(84, 0);
		OLEDprint(OLED_display_array[0]);
		IdleMode = 0;
	}
}

/*
 * @Function: Write_CanAddress
 * @Description: Write CanAddress from 1 - 9 to FLASH sector 31 , Space bank 1
 * @Parameters: Addr
 */
void Write_CanAddress(uint8_t Addr)
{
	memset(ContentArray, Addr, 1);
	FlashCtl_unprotectSector(FLASH_MAIN_MEMORY_SPACE_BANK1, FLASH_SECTOR31);
	FlashCtl_eraseSector(CANADDRESS_START);
	FlashCtl_programMemory(ContentArray, (void*) CANADDRESS_START, 1);
	FlashCtl_protectSector(FLASH_MAIN_MEMORY_SPACE_BANK1, FLASH_SECTOR31);

}

void OLED_Menu(void)
{
	static uint32_t OLEDtimeout = 0;
	uint8_t arrow_state = 0;

	// show Main Menu
	OLED_Main_Menu();
	arrow_state = move_arrow();
	SDCS_NW_ID = Read_CanAddress();
	// Arrow is pointed to Start option
	while (1) // Register 30s timeout for autostart
	{

		if (timeIsOut(OLEDtimeout, 300) || (SDCS_status & SDCS_WDTA_RESET))
		{
			registerTimeOut(&OLEDtimeout);
			if (!(SDCS_status & SDCS_CAN_ENABLE))
			{
				//setBit((uint8_t *)&SDCS_status, SDCS_CAN_ENABLE);
				SDCS_NW_ID = Read_CanAddress();
				can_rx_setfilter(0, 1, ((SDCS_NW_ID - 1) << 4 | 7));
				SDCS_status |= SDCS_CAN_ENABLE;
				OLED_Start_Page();

			}
			break;
		}

		// check for Button from interrupt
		if ((Button_Status & BUTTONPRESS))
		{
			//clearBit(&Button_Status, Button_press);
			Button_Status &= ~BUTTONPRESS;

			if (Button_Status & RIGHTBUTTON)
			{
				if (arrow_state == 1)
				{
					// Go to start Page
					OLED_Start_Page();
					// set new can filter
					can_rx_setfilter(0, 1, ((SDCS_NW_ID - 1) << 4 | 7));
					// set CAN enable bit in SDCS_status
					//setBit((uint8_t *)&SDCS_status, SDCS_CAN_ENABLE);
					SDCS_status |= SDCS_CAN_ENABLE;
					break;
				}
				else
				{
					// Go to change address page
					OLED_Address_Page();
					static uint8_t CAN_ADDR = 0;
					while (1)
					{

						if (Button_Status & BUTTONPRESS)
						{
							//clearBit(&Button_Status, Button_press);
							Button_Status &= ~BUTTONPRESS;
							if (Button_Status & RIGHTBUTTON)
							{
								// Go to start Page
								Write_CanAddress(CAN_ADDR - 1);
								SDCS_NW_ID = CAN_ADDR - 1;
								OLED_Start_Page();
								// write new SDCS ID to FLASH

								// set new can filter
								//Delay_1ms(1);
								can_rx_setfilter(0, 1,
										((SDCS_NW_ID - 1) << 4 | 7));
								// set CAN enable bit in SDCS_status
								//setBit((uint8_t *)&SDCS_status, SDCS_CAN_ENABLE);
								SDCS_status |= SDCS_CAN_ENABLE;
								break;
							}
							else
							{
								CAN_ADDR = OLED_Address_ChangeCANAddr();
							}
						}
					}
					break;
				}
			}
			else
			{
				arrow_state = move_arrow();
			}
		}
	}

	//clearBit((uint8_t *) SDCS_status, SDCS_WDTA_RESET);
	SDCS_status &= ~SDCS_WDTA_RESET;
	// check if Timeout -> auto start
	// disable button interrupt
	//MAP_Interrupt_disableInterrupt(INT_PORT5);
	SetupFinish = 1;

}
