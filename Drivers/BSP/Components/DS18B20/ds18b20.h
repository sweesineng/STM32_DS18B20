/**
  ******************************************************************************
  * @file    ds18b20.h
  * @brief   This file contains all the constants parameters for the DS18B20
  * 		 1-Wire Digital Thermometer
  ******************************************************************************
  * @attention
  * Usage:
  *		Uncomment LL Driver for HAL driver
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef DS18B20_H
#define DS18B20_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "onewire.h"

/* Data Structure ------------------------------------------------------------*/
#define DS18B20_MaxCnt		2

/* Register ------------------------------------------------------------------*/
#define DS18B20_CMD_CONVERT				0x44
#define DS18B20_CMD_ALARM_SEARCH		0xEC
#define DS18B20_CMD_READSCRATCHPAD		0xBE
#define DS18B20_CMD_WRITESCRATCHPAD		0x4E
#define DS18B20_CMD_COPYSCRATCHPAD		0x48
/* Data Structure ------------------------------------------------------------*/
#define DS18B20_FAMILY_CODE				0x28

/* Bits locations for resolution */
#define DS18B20_RESOLUTION_R1			6
#define DS18B20_RESOLUTION_R0			5

#define DS18B20_DECIMAL_STEPS_12BIT		0.0625
#define DS18B20_DECIMAL_STEPS_11BIT		0.125
#define DS18B20_DECIMAL_STEPS_10BIT		0.25
#define DS18B20_DECIMAL_STEPS_9BIT		0.5


/* DS18B20 Resolutions */
typedef enum {
	DS18B20_Resolution_9bits	= 9,
	DS18B20_Resolution_10bits	= 10,
	DS18B20_Resolution_11bits	= 11,
	DS18B20_Resolution_12bits	= 12
} DS18B20_Res_t;

typedef struct
{
	uint8_t 		DevAddr[DS18B20_MaxCnt][8];
	uint8_t 		AlmAddr[DS18B20_MaxCnt][8];
	float 			Temperature[DS18B20_MaxCnt];
	DS18B20_Res_t	Resolution;
} DS18B20_Drv_t;

/* External Function ---------------------------------------------------------*/
uint8_t DS18B20_Init(DS18B20_Drv_t *DS, OneWire_t *OW);
uint8_t DS18B20_Start(OneWire_t* OW, uint8_t *ROM);
void DS18B20_StartAll(OneWire_t* OW);
uint8_t DS18B20_Read(OneWire_t* OW, uint8_t *ROM, float *destination);
uint8_t DS18B20_SetTempAlarm(OneWire_t* OW, uint8_t *ROM, int8_t Low,
		int8_t High);
uint8_t DS18B20_AlarmSearch(DS18B20_Drv_t *DS, OneWire_t* OW);

#ifdef __cplusplus
}
#endif

#endif /* DS18B20_H */
