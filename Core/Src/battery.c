/**
  ******************************************************************************
  * @file    battery.c
  * @brief   Battery voltage monitor.
  *
  * @note    How it works:
  *          1. CheckBatteryPeriodic() runs in the main loop.
  *             It measures the battery only once every 10 seconds.
  *          2. It uses the ADC Injected channel. This pauses the main
  *             oscilloscope ADC for a moment, measures the battery,
  *             and goes back without losing graph data.
  *          3. We have a 1/2 voltage divider on the board.
  *             So, we read the ADC value, convert it to Volts,
  *             and multiply by 2 to get the real battery voltage.
  ******************************************************************************
  */

#include "battery.h"
#include "arm_math.h"

extern ADC_HandleTypeDef hadc1;

uint8_t is_battery_low = 0;
static uint32_t last_battery_check = 0;
float32_t battery_voltage = 0.0f;

void CheckBatteryPeriodic()
{
	uint32_t current_time = HAL_GetTick();

	if (current_time - last_battery_check < 10000)
	{
		return;
	}

	last_battery_check = current_time;

	HAL_ADCEx_InjectedStart(&hadc1);

	if (HAL_ADCEx_InjectedPollForConversion(&hadc1, 10) == HAL_OK)
	{
		uint32_t raw_bat = HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_1);
		battery_voltage = ((float)raw_bat * 3.3f / 4095.0f) * 2.0f;
	}

	if (battery_voltage <= 3.25f)
	{
		is_battery_low = 1;
	}
	else if (battery_voltage >= 3.35f)
	{
		is_battery_low = 0;
	}
}
