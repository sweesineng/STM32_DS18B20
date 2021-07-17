/**
  ******************************************************************************
  * @file    dwt.h
  * @brief   This file contains all the constants parameters for the dwt delay
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef DWT_H
#define DWT_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Custom Define -------------------------------------------------------------*/
#define  DWT_LAR_UNLOCK		(uint32_t)0xC5ACCE55
#define  DEM_CR_TRCENA		(1 << 24)
#define  DWT_CR_CYCCNTENA	(1 <<  0)
#define  DWT_CR				*(volatile uint32_t *)0xE0001000
#define  DWT_LAR			*(volatile uint32_t *)0xE0001FB0
#define  DWT_CYCCNT			*(volatile uint32_t *)0xE0001004
#define  DEM_CR				*(volatile uint32_t *)0xE000EDFC


/* External Function ---------------------------------------------------------*/
void DwtInit(void);
void DwtStart(void);
float DwtInterval(void);
void DwtDelay_us(uint32_t usec);
void DwtDelay_ms(uint32_t msec);

#ifdef __cplusplus
}
#endif

#endif /* DWT_H */
