/*
 * TimeOut.c
 *
 *  Created on: Aug 19, 2015
 *      Author: tam
 */

#include "TimeOut.h"
#include "OLEDlibrary/OLED.h"
#include "GaugeSensor/GaugeSensor.h"
/*
 * Handle: Timer0
 */
uint32_t Tick_counter = 0;
uint32_t Systickcnt = 0;

/*
 * @Function: systick_isr
 * @Description: systick interrupt service routine is called every 100ms.
 */

void systickisr()
{
	Systickcnt++;

	MAP_ADC14_toggleConversionTrigger();

	MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
}

void timera0isr()
{
	  MAP_Timer_A_clearCaptureCompareInterrupt(TIMER_A1_MODULE,
	            TIMER_A_CAPTURECOMPARE_REGISTER_0);
	Systickcnt++;

	MAP_ADC14_toggleConversionTrigger();

	MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
}
void startTimer()
{
	Tick_counter = Systickcnt;
}

uint32_t timerElapsed()
{
	return (uint32_t) (Systickcnt - Tick_counter);
}

void registerTimeOut(uint32_t * input)
{
	*input = Systickcnt;
}

uint8_t timeIsOut(uint32_t input, uint32_t timeInterval)
{
	if (((uint32_t) (Systickcnt - input)) >= timeInterval)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

uint32_t TITaskDelayUntil(uint32_t wakeTime, uint32_t timeInterval)
{
	uint32_t Dummy = Systickcnt;
	if ((long) (wakeTime + timeInterval - Dummy) < 0)
	{
		return 0;
	}
	else
	{
		return (wakeTime + timeInterval - Dummy);
	}
}

void Delay_50ms(uint32_t Interval)
{
	uint32_t counter = Systickcnt;
	while ((Systickcnt - counter) <= Interval)
		;

}
