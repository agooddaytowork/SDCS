/*
 * EDCTUComm.c
 *
 *  Created on: Jan 6, 2016
 *      Author: TamDuong
 */

#include "mcp2515.h"
#include "driverlib.h"
#include "../SDCS.h"
#include "../OLEDlibrary/OLED.h"
// CAN ISR
extern volatile uint8_t SDCS_NW_ID;
extern uint8_t DaTaPacket[7][8];
extern uint8_t PresencePacket;

/*
 * check TX success IRQ in MCP2515_CANINTF register
 */

void canisr()
{
	uint32_t status, rid;
	uint8_t irq, mext;
	uint8_t RXmsg[8];
	volatile uint8_t TxBuf[8];
	uint8_t Packetcnt = 0;
	uint8_t msgid = 0;

	status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P2);
	MAP_GPIO_clearInterruptFlag(GPIO_PORT_P2, status);

	if (SDCS_status & SDCS_CAN_ENABLE)
	{
		// Check interrupt on pin P2.7
		if (status & GPIO_PIN7)
		{
			// set internal flag of mcp2515
			mcp2515_irq |= MCP2515_IRQ_FLAGGED;
			// execute IRQ Handler
			irq = can_irq_handler();
			// check error and RX flag
			if ((irq & MCP2515_IRQ_RX) && !(irq & MCP2515_IRQ_ERROR))
			{
				// read RX packet
				can_recv(&rid, &mext, RXmsg);
				// Presence checking request
				if (rid == 0xF0)
				{
					msgid = ((SDCS_NW_ID - 1) << 4 | CAN_CONFIG_TAIL);
					can_send(msgid, 0, &PresencePacket, 1, 1); // DONE MISS THE & SYMBOL AGIAN PLSSSSS
				}

				// Data Request
				if (rid == ((SDCS_NW_ID - 1) << 4 | 7))
				{

					while (Packetcnt < 7)
					{
						irq = can_irq_handler();
						if (can_tx_available() != -1)
						{
							can_send(((SDCS_NW_ID - 1) << 4 | Packetcnt), 0,
									DaTaPacket[Packetcnt], 8, 3);
							Packetcnt++;
						}

					}

				}

				Packetcnt = 0; // Reset Packetcnt
			}
		}

	}
}


