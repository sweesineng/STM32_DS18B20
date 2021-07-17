/**
  ******************************************************************************
  * @file    dwt.c
  * @brief   This file includes the utilities for DWT
  ******************************************************************************
  */
#include "dwt.h"

static uint32_t SysCClk, start;

/**
  * @brief  Initialize DWT
  */
void DwtInit(void)
{
	SysCClk 		= (SystemCoreClock / 1000000);	// Calculate in us
	DWT_LAR			|= DWT_LAR_UNLOCK;
	DEM_CR			|= (uint32_t)DEM_CR_TRCENA;
	DWT_CYCCNT		= (uint32_t)0u;					// Reset the clock counter
	DWT_CR			|= (uint32_t)DWT_CR_CYCCNTENA;
}

/**
  * @brief  Start DWT Counter
  */
void DwtStart(void)
{
	start = DWT_CYCCNT;
}

/**
  * @brief  Calculate Interval Base On Previous Start Time
  * @retval Interval in us
  */
float DwtInterval(void)
{
	return (float)(DWT_CYCCNT - start) / SysCClk;
}

/**
  * @brief  Function to delay in microsecond
  * @param	usec	Period in microsecond
  */
inline void DwtDelay_us(uint32_t usec)
{
	start = DWT_CYCCNT;
	while(((DWT_CYCCNT - start) / SysCClk) < usec) {};
}

/**
  * @brief  Function to delay in millisecond
  * @param	msec	Period in millisecond
  */
inline void DwtDelay_ms(uint32_t msec)
{
	start = DWT_CYCCNT;
	while(((DWT_CYCCNT - start) / SysCClk) < (msec * 1000)) {};
}
