#include "oscilloscope.h"

static State current_state = OSC_WAIT_DATA;
static uint16_t adc_buffer[4096];
static uint8_t buffer_flag = 0;

void OscilloscopeUpdate(void);
void OscilloscopeToggleRunStop(void);
void OsclilloscopeTrigger(void);
void OscilloscopeSendData(void);
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
	current_state = OSC_WAIT_DATA;
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
