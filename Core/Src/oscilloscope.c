#include "oscilloscope.h"

static State current_state = OSC_WAIT_DATA;
static uint16_t adc_buffer[4096];
static uint16_t trigger_index = 0;
static uint8_t buffer_flag = 0;

void OscilloscopeUpdate(void);
void OscilloscopeToggleRunStop(void);
static void OsclilloscopeTrigger(void);
static void OscilloscopeSendData(void);
void HAL_ADC_ConvHalfCpltCallback (ADC_HandleTypeDef * hadc1);
void HAL_ADC_ConvCpltCallback (ADC_HandleTypeDef * hadc1);

void OscilloscopeUpdate()
{
	switch (current_state)
	{
	case OSC_WAIT_DATA:
		break;
	case OSC_FIND_TRIGGER:
		OsclilloscopeTrigger();
		break;
	case OSC_DRAW_SCREEN:
		OscilloscopeSendData();
		break;
	case OSC_PAUSED:
		break;
	}
}

void HAL_ADC_ConvHalfCpltCallback (ADC_HandleTypeDef *hadc1)
{
	if (current_state != OSC_WAIT_DATA)
	{
		return;
	}
	buffer_flag = 0;
	current_state = OSC_FIND_TRIGGER;
}

void HAL_ADC_ConvCpltCallback (ADC_HandleTypeDef *hadc1)
{
	if (current_state != OSC_WAIT_DATA)
	{
		return;
	}
	buffer_flag = 1;
	current_state = OSC_FIND_TRIGGER;
}

void OscilloscopeToggleRunStop()
{
	if (current_state == OSC_PAUSED)
	{
		current_state = OSC_WAIT_DATA;
	}
	else
	{
		current_state = OSC_PAUSED;
	}
}

void OscilloscopeTrigger()
{
	// Trigger was found
	uint16_t start_index = (buffer_flag == 0) ? 1 : 2049;
	uint16_t end_index = start_index + 2047 - 320;

	for (int i = start_index; i < end_index; i++)
	{
		if (adc_buffer[i-1] < 2048 && adc_buffer[i] >= 2048)
		{
			trigger_index = i;
			current_state = OSC_DRAW_SCREEN;
			break;
		}
	}

	// Trigger was not found
	if (current_state == OSC_FIND_TRIGGER)
	{
		trigger_index = (buffer_flag == 0) ? 0 : 2048;
		current_state = OSC_DRAW_SCREEN;
	}
}
