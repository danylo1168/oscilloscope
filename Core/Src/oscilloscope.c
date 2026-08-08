#include "oscilloscope.h"
#include "display.h"

static State current_state = OSC_WAIT_DATA;
static uint16_t adc_buffer[4096];
static uint16_t trigger_index = 0;
static uint8_t buffer_flag = 0;
static uint16_t old_y[320] = {0};
const uint32_t timebase_arr_values[6] = {99, 199, 499, 999, 1999, 4999};
static uint32_t last_encoder_val = 0;
static uint8_t timebase_index = 0;
const uint8_t volts_arr_values[4] = {1, 2, 5, 10};
static uint8_t volts_index = 0;
static ScaleControls scale_controls_mode = TIME;

extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim4;

void OscilloscopeUpdate(void);
void OscilloscopeToggleRunStop(void);
void OscilloscopeToggleMode(void);
static void OscilloscopeTrigger(void);
static void OscilloscopeSendData(void);
void HAL_ADC_ConvHalfCpltCallback (ADC_HandleTypeDef * hadc1);
void HAL_ADC_ConvCpltCallback (ADC_HandleTypeDef * hadc1);
void OscilloscopeInit(ADC_HandleTypeDef *hadc1);
void OscilloscopeUpdateEncoder();

void OscilloscopeUpdate()
{
	switch (current_state)
	{
	case OSC_WAIT_DATA:
		break;
	case OSC_FIND_TRIGGER:
		OscilloscopeTrigger();
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

static void OscilloscopeSendData()
{
	uint16_t raw_value = 0;
	int32_t calculated_value = 0;
	uint16_t y = 0;

	for (int i = 0; i < 320; i++) // X = i
	{
		raw_value = adc_buffer[trigger_index + i];

		calculated_value = raw_value - 2048;
		calculated_value *= volts_arr_values[volts_index];
		calculated_value += 2048;

		if (calculated_value < 0)
		{
			calculated_value = 0;
		}
		else if (calculated_value > 4095)
		{
			calculated_value = 4095;
		}

		// 0 < raw_value < 4096
		// 4096/240 = 256/15
	    // (raw_value * 15) / 256
		// 240 - (raw_value * 15) / 256 to flip Y coordinate
		y = 240 - ((calculated_value * 15) >> 8);

		// Draw new frame
		// DrawScreen(i, old_y[i], BLACK);
		// DrawScreen (i, y, GREEN);
		ScreenDrawPixel(i, old_y[i], BLACK);
		ScreenDrawPixel(i, y, GREEN);
		old_y[i] = y;

	}
	current_state = OSC_WAIT_DATA;
}

void OscilloscopeUpdateEncoder(void)
{
    uint32_t current_tim_value = __HAL_TIM_GET_COUNTER(&htim4);

    if (current_tim_value == last_encoder_val)
    {
        return;
    }

    int16_t diff = (int16_t)(current_tim_value - last_encoder_val);

    if (diff > 0)
    {
        if (scale_controls_mode == TIME && timebase_index < 5)
        {
            timebase_index++;
            __HAL_TIM_SET_AUTORELOAD(&htim2, timebase_arr_values[timebase_index]);
        }
        else if (scale_controls_mode == VOLTS && volts_index < 3)
        {
        	volts_index++;
        }
    }
    else if (diff < 0)
    {
        if (scale_controls_mode == TIME && timebase_index > 0)
        {
            timebase_index--;
            __HAL_TIM_SET_AUTORELOAD(&htim2, timebase_arr_values[timebase_index]);
        }
        else if (scale_controls_mode == VOLTS && volts_index > 0)
        {
        	volts_index--;
        }
    }

    last_encoder_val = current_tim_value;
}

void OscilloscopeToggleMode()
{
	scale_controls_mode = (scale_controls_mode + 1) % (VOLTS + 1);
}

static void OscilloscopeTrigger()
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

void OscilloscopeInit(ADC_HandleTypeDef *hadc1)
{
    HAL_ADC_Start_DMA(hadc1, (uint32_t*)adc_buffer, 4096);
}
