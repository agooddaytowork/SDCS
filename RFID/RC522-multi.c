/*
 * RC522.c - for msp432 Launchpad
 * Driver Library for RC522 cheap RFID reader
 * The library is mainly based on the cpp verison of Eelco Rouw
 *  Created on: Jul 3, 2015
 *      Author: Tam.Duong
 */

#include "RC522.h"
#include "string.h"
#include "../TimeOut.h"
// Multi
void RC522_CS_HIGH(const uint8_t RFIDCS)
{
	switch (RFIDCS)
	{
	case 0: //RFID1 select P4.4
		GPIO_setOutputHighOnPin(RFIDPORT1, RFIDCS1);
		break;
	case 1: // RFID2 select P4.7
		GPIO_setOutputHighOnPin(RFIDPORT2, RFIDCS2);
		break;
	case 2: // RFID3 select P5.4
		GPIO_setOutputHighOnPin(RFIDPORT3, RFIDCS3);
		break;
	case 3: // RFID4 select P5.5
		GPIO_setOutputHighOnPin(RFIDPORT4, RFIDCS4);
		break;
	case 4: // RFID5 select P4.6
		GPIO_setOutputHighOnPin(RFIDPORT5, RFIDCS5);
		break;
	case 5: // RFID6 select P4.1
		GPIO_setOutputHighOnPin(RFIDPORT6, RFIDCS6);
		break;
	default:
		break;
	}
}

void RC522_CS_LOW(const uint8_t RFIDCS)
{
	switch (RFIDCS)
	{
	case 0: //RFID1 select P4.4
		GPIO_setOutputLowOnPin(RFIDPORT1, RFIDCS1);
		break;
	case 1: // RFID2 select P4.7
		GPIO_setOutputLowOnPin(RFIDPORT2, RFIDCS2);
		break;
	case 2: // RFID3 select P5.4
		GPIO_setOutputLowOnPin(RFIDPORT3, RFIDCS3);
		break;
	case 3: // RFID4 select P5.5
		GPIO_setOutputLowOnPin(RFIDPORT4, RFIDCS4);
		break;
	case 4: // RFID5 select P4.6
		GPIO_setOutputLowOnPin(RFIDPORT5, RFIDCS5);
		break;
	case 5: // RFID6 select P4.1
		GPIO_setOutputLowOnPin(RFIDPORT6, RFIDCS6);
		break;
	default:
		break;
	}
}

void RC522_NRSTPD_HIGH()
{
	GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN5);
}

void RC522_NRSTPD_LOW()
{
	GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN5);
}

/*
 * Using UCB2
 * return 0 if Time is Out
 */
uint8_t RC522_SPI_transfer(uint8_t data)
{
	uint32_t dummyTime;
	UCB2TXBUF = data;

	registerTimeOut(&dummyTime);

	while (!(timeIsOut(dummyTime, 1)))
	{
		if ((SPI_getInterruptStatus(EUSCI_B2_MODULE,
				EUSCI_B_SPI_RECEIVE_INTERRUPT)))
			return UCB2RXBUF;
	}
//
//	 while (!(SPI_getInterruptStatus(EUSCI_B2_MODULE, EUSCI_B_SPI_TRANSMIT_INTERRUPT)));
//
//	 return UCB2RXBUF;

	//SPI_transmitData(EUSCI_B0_MODULE, data);
	//__delay_cycles(150);

	/*
	 while (!(UCB2IFG & UCRXIFG0))
	 ;
	 */
//	while(!(SPI_getInterruptStatus(EUSCI_B2_MODULE, EUSCI_B_SPI_RECEIVE_INTERRUPT)));
	return 0;
}
/*
 * Function: RC522_w_reg
 *input: register address, and write value, RFIDCS - 1->4
 *return: NULL
 */
void RC522_w_reg(uint8_t addr, uint8_t val, const uint8_t RFIDCS)
{
	RC522_CS_LOW(RFIDCS);
	RC522_SPI_transfer((addr << 1) & 0x7E);
	RC522_SPI_transfer(val);
	RC522_CS_HIGH(RFIDCS);

}
/*
 * Function: RC522_r_reg
 *Input: read register address, RFIDCS - 1->4
 *return: the read value
 */
uint8_t RC522_r_reg(uint8_t addr, const uint8_t RFIDCS)
{
	uint8_t val;
	RC522_CS_LOW(RFIDCS);
	RC522_SPI_transfer(((addr << 1) & 0x7E) | 0x80);
	val = RC522_SPI_transfer(0x00);
	RC522_CS_HIGH(RFIDCS);
	return val;
}
/*
 * Function: RC522_SetBitMask
 * Input: register address, mask - value, RFIDCS
 * return: NULL
 */
void RC522_SetBitMask(uint8_t reg, uint8_t mask, const uint8_t RFIDCS)
{
	uint8_t tmp;
	tmp = RC522_r_reg(reg, RFIDCS);
	RC522_w_reg(reg, tmp | mask, RFIDCS);
}
/*
 * Function: RC522_ClearBitMask
 * input: register address, mask-value, RFIDCS
 * return: NULL
 */
void RC522_ClearBitMask(uint8_t reg, uint8_t mask, const uint8_t RFIDCS)
{
	uint8_t tmp;
	tmp = RC522_r_reg(reg, RFIDCS);
	RC522_w_reg(reg, tmp & (~mask), RFIDCS);
}

/*
 * Function: RC522_AntennaOn
 * Description: Turn on antenna, wait 1ms for stabilizing,RFIDCS
 * return: NULL
 */
void RC522_AntennaOn(const uint8_t RFIDCS)
{
	uint8_t tmp;
	tmp = RC522_r_reg(TxControlReg, RFIDCS);
	if (!(tmp & 0x03))
	{
		RC522_SetBitMask(TxControlReg, 0x03, RFIDCS);
		//__delay_cycles(50000); // ~1ms
	}
}

/*
 * Function: RC522_AntennaOff
 * Input: RFIDCS
 * Description: Turn antenna off, wait 1ms for stabilizing
 * return: NULL
 */
void RC522_AntennaOff(const uint8_t RFIDCS)
{
	RC522_ClearBitMask(TxControlReg, 0x03, RFIDCS);
}
/*
 * Function: RC522_Reset
 * Input: RFIDCS
 * Description: reset RC522
 */
void RC522_Reset(const uint8_t RFIDCS)
{
	RC522_w_reg(CommandReg, PCD_RESETPHASE, RFIDCS);
}

void RC522_Indi_Init(const uint8_t RFIDCS)
{
	RC522_Reset(RFIDCS);
	//__delay_cycles(100000);

	//Timer: TPrescaler*TreloadVal/6.78MHz = 24ms

	RC522_w_reg(TModeReg, 0x8D, RFIDCS); //Tauto=1; f(Timer) = 6.78MHz/TPreScaler
	RC522_w_reg(TPrescalerReg, 0x3E, RFIDCS); //TModeReg[3..0] + TPrescalerReg
	RC522_w_reg(TReloadRegL, 30, RFIDCS);
	RC522_w_reg(TReloadRegH, 0, RFIDCS);

	RC522_w_reg(TxAutoReg, 0x40, RFIDCS);		//100%ASK
	RC522_w_reg(ModeReg, 0x3D, RFIDCS);

	//RC522_AntennaOn(RFIDCS);
	RC522_AntennaOff(RFIDCS);
}

const eUSCI_SPI_MasterConfig spiRC522MasterConfig =
{
EUSCI_B_SPI_CLOCKSOURCE_SMCLK,              //  SPI Source Clock = SMCLK
		24000000,                                     // SMCLK = 24MHz
		450000,                                    // SPICLK = 300Khz
		EUSCI_B_SPI_MSB_FIRST,                     // MSB First
		EUSCI_B_SPI_PHASE_DATA_CAPTURED_ONFIRST_CHANGED_ON_NEXT,    // Phase
		EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_LOW, // Low polarity
		EUSCI_B_SPI_3PIN                           // 3Wire SPI Mode
		};
/*
 * Function: RC522_Init
 * Description: Initialize RC522
 */
void RC522_Init()
{
	//int RFIDCS = 1;
	// RFIDCS1 P4.4, inactive HIGH
	GPIO_setAsOutputPin(RFIDPORT1, RFIDCS1);
	GPIO_setOutputHighOnPin(RFIDPORT1, RFIDCS1);
	// RFIDCS2 P4.7, inactive HIGH
	GPIO_setAsOutputPin(RFIDPORT2, RFIDCS2);
	GPIO_setOutputHighOnPin(RFIDPORT2, RFIDCS2);
	// RFIDCS3 P5.4, inactive HIGH
	GPIO_setAsOutputPin(RFIDPORT3, RFIDCS3);
	GPIO_setOutputHighOnPin(RFIDPORT3, RFIDCS3);
	// RFIDCS4 P5.5, inactive HIGH
	GPIO_setAsOutputPin(RFIDPORT4, RFIDCS4);
	GPIO_setOutputHighOnPin(RFIDPORT4, RFIDCS4);
	// RFIDCS5 P4.6, inactive HIGH
	GPIO_setAsOutputPin(RFIDPORT5, RFIDCS5);
	GPIO_setOutputHighOnPin(RFIDPORT5, RFIDCS5);
	// RFIDCS6 P4.1, inactive HIGH
	GPIO_setAsOutputPin(RFIDPORT6, RFIDCS6);
	GPIO_setOutputHighOnPin(RFIDPORT6, RFIDCS6);
	// NRSTPD pin P4.5, inactive HIGH
	GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN5);
	GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN5);
	__delay_cycles(1000000);
	GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN5);

	/* Selecting P3.5 P3.6 and P3.7 in SPI mode */
	GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P3,
	GPIO_PIN6 | GPIO_PIN7 | GPIO_PIN5, GPIO_PRIMARY_MODULE_FUNCTION);
	/* Configuring SPI EUSCI_B2_MODULE in 3wire master mode */
	SPI_initMaster(EUSCI_B2_MODULE, &spiRC522MasterConfig);

	/* Enable SPI module */
	SPI_enableModule(EUSCI_B2_MODULE);
	__delay_cycles(20000);

	/*
	 for (RFIDCS = 1; RFIDCS < 7; RFIDCS++)
	 {
	 // Reset Chip before Intializing
	 RC522_Reset(RFIDCS);
	 //__delay_cycles(100000);

	 //Timer: TPrescaler*TreloadVal/6.78MHz = 24ms

	 RC522_w_reg(TModeReg, 0x8D, RFIDCS); //Tauto=1; f(Timer) = 6.78MHz/TPreScaler
	 RC522_w_reg(TPrescalerReg, 0x3E, RFIDCS); //TModeReg[3..0] + TPrescalerReg
	 RC522_w_reg(TReloadRegL, 30, RFIDCS);
	 RC522_w_reg(TReloadRegH, 0, RFIDCS);

	 RC522_w_reg(TxAutoReg, 0x40, RFIDCS);		//100%ASK
	 RC522_w_reg(ModeReg, 0x3D, RFIDCS);

	 RC522_AntennaOn(RFIDCS);
	 }
	 */
	//RC522_Indi_Init(1);
	//__delay_cycles(20000);
	//RC522_Indi_Init(3);
	//RC522_Indi_Init(5);
	//RC522_Indi_Init(2);
}

/*
 * Function: RC522_Request
 * Description: Searching card, read card type
 * Input parameter：reqMode--search methods，
 *			 TagType--return card types
 *			 	0x4400 = Mifare_UltraLight
 *				0x0400 = Mifare_One(S50)
 *				0x0200 = Mifare_One(S70)
 *				0x0800 = Mifare_Pro(X)
 *				0x4403 = Mifare_DESFire
 * return：return MI_OK if successed
 */
uint8_t RC522_Request(uint8_t reqMode, uint8_t *TagType,const uint8_t RFIDCS)
{
	uint8_t status;
	uint8_t backBits;

	RC522_w_reg(BitFramingReg, 0x07, RFIDCS); //TxLastBists = BitFramingReg[2..0]	???

	TagType[0] = reqMode;
	status = RC522_ToCard(PCD_TRANSCEIVE, TagType, 1, TagType, &backBits,
			RFIDCS);

	if ((status != MI_OK) || (backBits != 0x10))
	{
		status = MI_ERR;
	}

	return status;
}

/*
 * Function: RC522_ToCard
 * Description: communicate between RC522 and ISO14443
 * * Input parameter：command--MF522 command bits
 *			 sendData--send data to card via rc522
 *			 sendLen--send data length
 *			 backData--the return data from card
 *			 backLen--the length of return data
 *			  return：return MI_OK if successed
 */
uint8_t RC522_ToCard(uint8_t command, uint8_t *sendData, uint8_t sendLen,
		uint8_t *backData, uint8_t *backLen, const uint8_t RFIDCS)
{
	uint8_t status = MI_ERR;
	uint8_t irqEn = 0x00;
	uint8_t waitIRq = 0x00;
	uint8_t lastBits;
	uint8_t n;
	uint32_t i;

	switch (command)
	{
	case PCD_AUTHENT:
	{
		irqEn = 0x12;
		waitIRq = 0x10;
		break;
	}
	case PCD_TRANSCEIVE:
	{
		irqEn = 0x77;
		waitIRq = 0x30;
		break;
	}
	default:
		break;
	}

	RC522_w_reg(CommIEnReg, irqEn | 0x80, RFIDCS); // Communication interrupt Enable register ,
	RC522_ClearBitMask(CommIrqReg, 0x80, RFIDCS);
	RC522_SetBitMask(FIFOLevelReg, 0x80, RFIDCS);

	RC522_w_reg(CommandReg, PCD_IDLE, RFIDCS);

	for (i = 0; i < sendLen; i++)
	{
		RC522_w_reg(FIFODataReg, sendData[i], RFIDCS);
	}

	RC522_w_reg(CommandReg, command, RFIDCS);
	if (command == PCD_TRANSCEIVE)
	{
		RC522_SetBitMask(BitFramingReg, 0x80, RFIDCS); //StartSend=1,transmission of data starts
	}
	i = 2000;
	do
	{
		n = RC522_r_reg(CommIrqReg, RFIDCS);
		i--;
	} while ((i != 0) && !(n & 0x01) && !(n & waitIRq)); // Cau truc nay co tren C khong troi ???

	RC522_ClearBitMask(BitFramingReg, 0x80, RFIDCS); // StartSend = 0

	if (i != 0)
	{
		if (!(RC522_r_reg(ErrorReg, RFIDCS) & 0x1B)) //BufferOvfl Collerr CRCErr ProtecolErr
		{
			status = MI_OK;
			if (n & irqEn & 0x01)
			{
				status = MI_NOTAGERR;			//??
			}

			if (command == PCD_TRANSCEIVE)
			{
				n = RC522_r_reg(FIFOLevelReg, RFIDCS);
				lastBits = RC522_r_reg(ControlReg, RFIDCS) & 0x07;
				if (lastBits)
				{
					*backLen = (n - 1) * 8 + lastBits;
				}
				else
				{
					*backLen = n * 8;
				}
				if (n == 0)
				{
					n = 1;
				}
				if (n > MAX_LEN)
				{
					n = MAX_LEN;
				}

				for (i = 0; i < n; i++)
				{
					backData[i] = RC522_r_reg(FIFODataReg, RFIDCS);
				}
			}
		}
		else
		{
			status = MI_ERR;
		}
	}

	return status;
}

/*
 * Function: RC522_Anticoll
 * Description：Prevent conflict, read the card serial number
 * Input parameter：serNum--return the 4 bytes card serial number, the 5th byte is recheck byte
 * return：return MI_OK if successed
 */
uint8_t RC522_Anticoll(uint8_t *serNum, const uint8_t RFIDCS)
{
	uint8_t status;
	uint8_t i;
	uint8_t serNumCheck = 0;

	uint8_t unLen;

	RC522_w_reg( BitFramingReg, 0x00, RFIDCS); //TxLastBists = BitFramingReg[2..0]
	serNum[0] = PICC_ANTICOLL;
	serNum[1] = 0x20;
	status = RC522_ToCard(PCD_TRANSCEIVE, serNum, 2, serNum, &unLen, RFIDCS);

	if (status == MI_OK)
	{
		for (i = 0; i < 4; i++)
		{
			serNumCheck ^= serNum[i];
		}
		if (serNumCheck != serNum[i])
		{
			status = MI_ERR;
		}
	}

	//SetBitMask(CollReg, 0x80);		//ValuesAfterColl=1

	return status;

}

/*
 * Function: RC522_Calculate
 *  Description：Use MF522 to calculate CRC
 * Input parameter：pIndata--the CRC data need to be read，len--data length，pOutData-- the caculated result of CRC
 * return：Null
 */
void RC522_CalculateCRC(uint8_t *pIndata, uint8_t len, uint8_t *pOutData,
		const uint8_t RFIDCS)
{
	uint8_t i, n;

	RC522_ClearBitMask(DivIrqReg, 0x04, RFIDCS);
	RC522_SetBitMask(FIFOLevelReg, 0x80, RFIDCS);

	for (i = 0; i < len; i++)
	{
		RC522_w_reg(FIFODataReg, *(pIndata + i), RFIDCS);
	}
	RC522_w_reg(CommandReg, PCD_CALCCRC, RFIDCS);

	i = 0xFF;
	do
	{
		n = RC522_r_reg(DivIrqReg, RFIDCS);
		i--;
	} while ((i != 0) && !(n & 0x04));			//CRCIrq = 1

	pOutData[0] = RC522_r_reg(CRCResultRegL, RFIDCS);
	pOutData[1] = RC522_r_reg(CRCResultRegM, RFIDCS);
}
/*
 * FunctionP RC522_SelectTag
 * Description：Select card, read card storage volume
 * Input parameter：serNum--Send card serial number
 * return：return the card storage volume
 */
uint8_t RC522_SelectTag(uint8_t *serNum, const uint8_t RFIDCS)
{
	uint8_t i;
	uint8_t status;
	uint8_t size;
	uint8_t recvBits;
	uint8_t buffer[9];

	//ClearBitMask(Status2Reg, 0x08);			//MFCrypto1On=0

	buffer[0] = PICC_SElECTTAG;
	buffer[1] = 0x70;
	for (i = 0; i < 5; i++)
	{
		buffer[i + 2] = *(serNum + i);
	}
	RC522_CalculateCRC(buffer, 7, &buffer[7], RFIDCS);		//??
	status = RC522_ToCard(PCD_TRANSCEIVE, buffer, 9, buffer, &recvBits, RFIDCS);

	if ((status == MI_OK) && (recvBits == 0x18))
	{
		size = buffer[0];
	}
	else
	{
		size = 0;
	}

	return size;
}

/*
 * Function: RC522_Auth
 * Description：verify card password
 * Input parameters：authMode--password verify mode
 0x60 = verify A password key
 0x61 = verify B password key
 BlockAddr--Block address
 Sectorkey--Block password
 serNum--Card serial number ，4 bytes
 * return：return MI_OK if successed
 */
uint8_t RC522_Auth(uint8_t authMode, uint8_t BlockAddr, uint8_t *Sectorkey,
		uint8_t *serNum, const uint8_t RFIDCS)
{
	uint8_t status;
	uint8_t recvBits;
	uint8_t i;
	uint8_t buff[12];

	buff[0] = authMode;
	buff[1] = BlockAddr;
	for (i = 0; i < 6; i++)
	{
		buff[i + 2] = *(Sectorkey + i);
	}
	for (i = 0; i < 4; i++)
	{
		buff[i + 8] = *(serNum + i);
	}
	status = RC522_ToCard(PCD_AUTHENT, buff, 12, buff, &recvBits, RFIDCS);

	if ((status != MI_OK) || (!(RC522_r_reg(Status2Reg, RFIDCS) & 0x08)))
	{
		status = MI_ERR;
	}

	return status;
}

/*
 * Function: RC522_ReadBlock
 * Description：Read data
 * Input parameters：blockAddr--block address;recvData--the block data which are read
 * return：return MI_OK if successed
 */
uint8_t RC522_ReadBlock(uint8_t blockAddr, uint8_t *recvData, const uint8_t RFIDCS)
{
	uint8_t status;
	uint8_t unLen;

	recvData[0] = PICC_READ;
	recvData[1] = blockAddr;
	RC522_CalculateCRC(recvData, 2, &recvData[2], RFIDCS);
	status = RC522_ToCard(PCD_TRANSCEIVE, recvData, 4, recvData, &unLen,
			RFIDCS);

	if ((status != MI_OK) || (unLen != 0x90))
	{
		status = MI_ERR;
	}

	return status;
}

/*
 * Function: RC522_WriteBlock
 * Description：write block data
 * Input parameters：blockAddr--block address;writeData--Write 16 bytes data into block
 * return：return MI_OK if successed
 *
 */
uint8_t RC522_WriteBlock(uint8_t blockAddr, uint8_t *writeData, const uint8_t RFIDCS)
{
	uint8_t status;
	uint8_t recvBits;
	uint8_t i;
	uint8_t buff[18];

	buff[0] = PICC_WRITE;
	buff[1] = blockAddr;
	RC522_CalculateCRC(buff, 2, &buff[2], RFIDCS);
	status = RC522_ToCard(PCD_TRANSCEIVE, buff, 4, buff, &recvBits, RFIDCS);

	if ((status != MI_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A))
	{
		status = MI_ERR;
	}

	if (status == MI_OK)
	{
		for (i = 0; i < 16; i++)
		{
			buff[i] = *(writeData + i);
		}
		RC522_CalculateCRC(buff, 16, &buff[16], RFIDCS);
		status = RC522_ToCard(PCD_TRANSCEIVE, buff, 18, buff, &recvBits,
				RFIDCS);

		if ((status != MI_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A))
		{
			status = MI_ERR;
		}
	}

	return status;
}

void RC522_Halt(const uint8_t RFIDCS)
{
	uint8_t status;
	uint8_t unLen;
	uint8_t buff[4];

	buff[0] = PICC_HALT;
	buff[1] = 0;
	RC522_CalculateCRC(buff, 2, &buff[2], RFIDCS);

	status = RC522_ToCard(PCD_TRANSCEIVE, buff, 4, buff, &unLen, RFIDCS);
	if (status == MI_OK)
		; // do nothing LOL
}

/*
 * RFID_ReadTag
 * parameter: RFIDCS
 *return 0, and a UID of 00-00-00-00 if No tag is read
 *return 1 and write the SerialNumber to User Array if a tag is read
 */
uint8_t RFID_ReadTag(const uint8_t RFIDCS, uint8_t *buf)
{
	unsigned char status;
	unsigned char str[MAX_LEN];

	//memset((uint8_t *) buf, 0, 5);
	RC522_Indi_Init(RFIDCS);
	RC522_AntennaOn(RFIDCS);
	// check if any Tag is on available
	status = RC522_Request(PICC_REQIDL, str, RFIDCS);
	if (status == MI_OK)
	{
		// read the tag UID
		status = RC522_Anticoll(str, RFIDCS);
		if (status == MI_OK)
		{
			memcpy((uint8_t *) buf, str, 5);
			RC522_Halt(RFIDCS);
			RC522_AntennaOff(RFIDCS);
			RC522_Reset(RFIDCS);
			return 1;
		}
	}
	RC522_Halt(RFIDCS);
	RC522_AntennaOff(RFIDCS);
	RC522_Reset(RFIDCS);
	return 0;
}
