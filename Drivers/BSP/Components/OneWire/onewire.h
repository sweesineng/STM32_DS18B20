/**
  ******************************************************************************
  * @file    onewire.h
  * @brief   This file contains all the constants parameters for the OneWire
  ******************************************************************************
  * @attention
  * Usage:
  *		Uncomment LL Driver for HAL driver
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef ONEWIRE_H
#define ONEWIRE_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Driver Selection ----------------------------------------------------------*/
//#define LL_Driver

/* Common Register -----------------------------------------------------------*/
#define ONEWIRE_CMD_SEARCHROM			0xF0
#define ONEWIRE_CMD_READROM				0x33
#define ONEWIRE_CMD_MATCHROM			0x55
#define ONEWIRE_CMD_SKIPROM				0xCC

/* Data Structure ------------------------------------------------------------*/
typedef enum
{
	Input,
	Output
} PinMode;

typedef struct
{
	uint8_t 		LastDiscrepancy;
	uint8_t 		LastFamilyDiscrepancy;
	uint8_t 		LastDeviceFlag;
	uint8_t			RomByte[8];
	uint8_t 		RomCnt;
	uint16_t		DataPin;
	GPIO_TypeDef	*DataPort;
} OneWire_t;

/* External Function ---------------------------------------------------------*/
void OneWire_Init(OneWire_t* OW);
uint8_t OneWire_Search(OneWire_t* OW, uint8_t Cmd);
void OneWire_GetDevRom(OneWire_t* OW, uint8_t *dev);
uint8_t OneWire_Reset(OneWire_t* OW);
uint8_t OneWire_ReadBit(OneWire_t* OW);
uint8_t OneWire_ReadByte(OneWire_t* OW);
void OneWire_WriteByte(OneWire_t* OW, uint8_t byte);
void OneWire_SelectWithPointer(OneWire_t* OW, uint8_t *Rom);
uint8_t OneWire_CRC8(uint8_t *addr, uint8_t len);

#ifdef __cplusplus
}
#endif

#endif /* ONEWIRE_H */
