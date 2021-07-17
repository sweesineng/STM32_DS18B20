/**
  ******************************************************************************
  * @file    onewire.c
  * @brief   This file includes the HAL/LL driver for OneWire devices
  ******************************************************************************
  */
#include "onewire.h"

/**
  * @brief  The internal function is used as gpio pin mode
  * @param  OW		OneWire HandleTypedef
  * @param  Mode	Input or Output
  */
static void OneWire_Pin_Mode(OneWire_t* OW, PinMode Mode)
{
#ifdef LL_Driver
	if(Mode == Input)
	{
		LL_GPIO_SetPinMode(OW->DataPort, OW->DataPin, LL_GPIO_MODE_INPUT);
	}else{
		LL_GPIO_SetPinMode(OW->DataPort, OW->DataPin, LL_GPIO_MODE_OUTPUT);
	}
#else
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = OW->DataPin;
	if(Mode == Input)
	{
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	}else{
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	}
	HAL_GPIO_Init(OW->DataPort, &GPIO_InitStruct);
#endif
}

/**
  * @brief  The internal function is used as gpio pin level
  * @param  OW		OneWire HandleTypedef
  * @param  Mode	Level: Set/High = 1, Reset/Low = 0
  */
static void OneWire_Pin_Level(OneWire_t* OW, uint8_t Level)
{
#ifdef LL_Driver
	if(Level == 1)
	{
		LL_GPIO_SetOutputPin(OW->DataPort, OW->DataPin);
	}else{
		LL_GPIO_ResetOutputPin(OW->DataPort, OW->DataPin);
	}
#else
	HAL_GPIO_WritePin(OW->DataPort, OW->DataPin, Level);
#endif
}

/**
  * @brief  The internal function is used to read data pin
  * @retval Pin level status
  * @param  OW		OneWire HandleTypedef
  */
static uint8_t OneWire_Pin_Read(OneWire_t* OW)
{
#ifdef LL_Driver
	return ((OW->DataPort->IDR & OW->DataPin) != 0x00U) ? 1 : 0;
#else
	return HAL_GPIO_ReadPin(OW->DataPort, OW->DataPin);
#endif
}

/**
  * @brief  The internal function is used to write bit
  * @param  OW		OneWire HandleTypedef
  * @param  bit		bit in 0 or 1
  */
static void OneWire_WriteBit(OneWire_t* OW, uint8_t bit)
{
	if(bit)
	{
		/* Set line low */
		OneWire_Pin_Level(OW, 0);
		OneWire_Pin_Mode(OW, Output);
		DwtDelay_us(10);

		/* Bit high */
		OneWire_Pin_Mode(OW, Input);

		/* Wait for 55 us and release the line */
		DwtDelay_us(55);
		OneWire_Pin_Mode(OW, Input);
	}else{
		/* Set line low */
		OneWire_Pin_Level(OW, 0);
		OneWire_Pin_Mode(OW, Output);
		DwtDelay_us(65);

		/* Bit high */
		OneWire_Pin_Mode(OW, Input);

		/* Wait for 5 us and release the line */
		DwtDelay_us(5);
		OneWire_Pin_Mode(OW, Input);
	}
}

/**
  * @brief  The function is used to read bit
  * @retval bit
  * @param  OW		OneWire HandleTypedef
  */
uint8_t OneWire_ReadBit(OneWire_t* OW)
{
	uint8_t bit = 0;

	/* Line low */
	OneWire_Pin_Level(OW, 0);
	OneWire_Pin_Mode(OW, Output);
	DwtDelay_us(3);

	/* Release line */
	OneWire_Pin_Mode(OW, Input);
	DwtDelay_us(10);

	/* Read line value */
	if (OneWire_Pin_Read(OW))
	{
		/* Bit is HIGH */
		bit = 1;
	}

	/* Wait 50us to complete 60us period */
	DwtDelay_us(50);

	/* Return bit value */
	return bit;
}

/**
  * @brief  The function is used to write byte
  * @param  OW		OneWire HandleTypedef
  * @param  byte	byte to write
  */
void OneWire_WriteByte(OneWire_t* OW, uint8_t byte)
{
	uint8_t bit = 8;
	/* Write 8 bits */
	while (bit--) {
		/* LSB bit is first */
		OneWire_WriteBit(OW, byte & 0x01);
		byte >>= 1;
	}
}

/**
  * @brief  The function is used to read byte
  * @retval byte from device
  * @param  OW		OneWire HandleTypedef
  */
uint8_t OneWire_ReadByte(OneWire_t* OW)
{
	uint8_t bit = 8, byte = 0;
	while (bit--) {
		byte >>= 1;
		byte |= (OneWire_ReadBit(OW) << 7);
	}

	return byte;
}

/**
  * @brief  The function is used to reset device
  * @retval respond from device
  * @param  OW		OneWire HandleTypedef
  */
uint8_t OneWire_Reset(OneWire_t* OW)
{
	/* Line low, and wait 480us */
	OneWire_Pin_Level(OW, 0);
	OneWire_Pin_Mode(OW, Output);
	DwtDelay_us(480);

	/* Release line and wait for 70us */
	OneWire_Pin_Mode(OW, Input);
	DwtDelay_us(70);

	/* Check bit value */
	uint8_t rslt = OneWire_Pin_Read(OW);

	/* Delay for 410 us */
	DwtDelay_us(410);

	return rslt;
}

/**
  * @brief  The function is used to search device
  * @retval Search result
  * @param  OW		OneWire HandleTypedef
  */
uint8_t OneWire_Search(OneWire_t* OW, uint8_t Cmd)
{
	uint8_t id_bit_number 	= 1;
	uint8_t last_zero 		= 0;
	uint8_t rom_byte_number = 0;
	uint8_t search_result 	= 0;
	uint8_t rom_byte_mask 	= 1;
	uint8_t id_bit, cmp_id_bit, search_direction;

	/* if the last call was not the last one */
	if (!OW->LastDeviceFlag)
	{
		if (OneWire_Reset(OW))
		{
			OW->LastDiscrepancy = 0;
			OW->LastDeviceFlag = 0;
			OW->LastFamilyDiscrepancy = 0;
			return 0;
		}

		// issue the search command
		OneWire_WriteByte(OW, Cmd);

		// loop to do the search
		do {
			// read a bit and its complement
			id_bit = OneWire_ReadBit(OW);
			cmp_id_bit = OneWire_ReadBit(OW);

			// check for no devices on 1-wire
			if ((id_bit == 1) && (cmp_id_bit == 1))
			{
				break;
			} else {
				// all devices coupled have 0 or 1
				if (id_bit != cmp_id_bit)
				{
					search_direction = id_bit;  // bit write value for search
				} else {
					/* if this discrepancy if before the Last Discrepancy
					 * on a previous next then pick the same as last time */
					if (id_bit_number < OW->LastDiscrepancy)
					{
						search_direction = ((OW->RomByte[rom_byte_number] & rom_byte_mask) > 0);
					} else {
						// if equal to last pick 1, if not then pick 0
						search_direction = (id_bit_number == OW->LastDiscrepancy);
					}

					// if 0 was picked then record its position in LastZero
					if (search_direction == 0)
					{
						last_zero = id_bit_number;

						// check for Last discrepancy in family
						if (last_zero < 9)
						{
							OW->LastFamilyDiscrepancy = last_zero;
						}
					}
				}

				/* set or clear the bit in the ROM byte rom_byte_number
				 * with mask rom_byte_mask */
				if (search_direction == 1)
				{
					OW->RomByte[rom_byte_number] |= rom_byte_mask;
				} else {
					OW->RomByte[rom_byte_number] &= ~rom_byte_mask;
				}

				// serial number search direction write bit
				OneWire_WriteBit(OW, search_direction);

				/* increment the byte counter id_bit_number and shift the
				 * mask rom_byte_mask */
				id_bit_number++;
				rom_byte_mask <<= 1;

				/* if the mask is 0 then go to new SerialNum byte
				 * rom_byte_number and reset mask */
				if (rom_byte_mask == 0)
				{
					rom_byte_number++;
					rom_byte_mask = 1;
				}
			}
		} while (rom_byte_number < 8);  /* loop until through all ROM bytes 0-7
		if the search was successful then */

		if (!(id_bit_number < 65))
		{
			/* search successful so set LastDiscrepancy, LastDeviceFlag,
			 * search_result */
			OW->LastDiscrepancy = last_zero;
			// check for last device
			if (OW->LastDiscrepancy == 0) {
				OW->LastDeviceFlag = 1;
			}
			search_result = 1;
		}
	}

	/* if no device found then reset counters so next 'search' will be like a
	 * first */
	if (!search_result || !OW->RomByte[0])
	{
		OW->LastDiscrepancy = 0;
		OW->LastDeviceFlag = 0;
		OW->LastFamilyDiscrepancy = 0;
		search_result = 0;
	}

	return search_result;
}

/**
  * @brief  The function is used get ROM full address
  * @param  OW		OneWire HandleTypedef
  * @param  ROM		Pointer to device ROM
  */
void OneWire_GetDevRom(OneWire_t* OW, uint8_t *ROM)
{
	for (uint8_t i = 0; i < 8; i++) {
		*(ROM + i) = OW->RomByte[i];
	}
}

/**
  * @brief  The function is used to initialize OneWire Communication
  * @param  OW		OneWire HandleTypedef
  */
void OneWire_Init(OneWire_t* OW)
{
	OneWire_Pin_Mode(OW, Output);
	OneWire_Pin_Level(OW, 1);
	DwtDelay_us(1000);
	OneWire_Pin_Level(OW, 0);
	DwtDelay_us(1000);
	OneWire_Pin_Level(OW, 1);
	DwtDelay_us(2000);

	/* Reset the search state */
	OW->LastDiscrepancy 		= 0;
	OW->LastDeviceFlag 			= 0;
	OW->LastFamilyDiscrepancy 	= 0;
	OW->RomCnt 					= 0;
}

/**
  * @brief  The function is used selected specific device ROM
  * @param  OW		OneWire HandleTypedef
  * @param  ROM		Pointer to device ROM
  */
void OneWire_SelectWithPointer(OneWire_t* OW, uint8_t *ROM)
{
	OneWire_WriteByte(OW, ONEWIRE_CMD_MATCHROM);

	for (uint8_t i = 0; i < 8; i++)
	{
		OneWire_WriteByte(OW, *(ROM + i));
	}
}

/**
  * @brief  The function is used check CRC
  * @param  Addr	Pointer to address
  * @param  ROM		Number of byte
  */
uint8_t OneWire_CRC8(uint8_t *Addr, uint8_t Len)
{
	uint8_t crc = 0;
	uint8_t inbyte, i, mix;

	while (Len--)
	{
		inbyte = *Addr++;

		for (i = 8; i; i--)
		{
			mix = (crc ^ inbyte) & 0x01;
			crc >>= 1;
			crc ^= (mix) ? 0x8C : 0;
			inbyte >>= 1;
		}
	}
	return crc;
}
