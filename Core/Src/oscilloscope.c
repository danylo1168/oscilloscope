#include "oscilloscope.h"
#include "ili9341.h"
#include "menu.h"
#include <stdio.h>

extern ADC_HandleTypeDef hadc1;

static State current_state = OSC_WAIT_DATA;
uint16_t adc_buffer[4096];
static uint16_t trigger_index = 0;
static uint8_t buffer_flag = 0;
static uint16_t old_y[320] = {0};
const uint32_t timebase_arr_values[6] = {99, 199, 499, 999, 1999, 4999};
static uint32_t last_encoder_val = 0;
static uint8_t timebase_index = 0;
const uint8_t volts_arr_values[4] = {1, 2, 5, 10};
static uint8_t volts_index = 0;
static ScaleControls scale_controls_mode = TIME;
static int32_t trigger_level = 2048;
int32_t zero_offset = 2048;

const char* timebase_strings[6] = {"1ms/d", "2ms/d", "5ms/d", "10ms/d", "20ms/d", "50ms/d"};
const char* volts_strings[4] = {"1V/div", "2V/div", "5V/div", "10V/d"};

extern char last_mode[20];
extern uint8_t spectrum_data_ready;

extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim4;

void OscilloscopeUpdate(void);
void OscilloscopeToggleRunStop(void);
void OscilloscopeToggleMode(void);
static void OscilloscopeTrigger(void);
static void OscilloscopeSendData(void);
void OscilloscopeCalibrateZero(void);
void OscilloscopeDrawGrid(void);
void OscilloscopeDrawHUD(void);
void HAL_ADC_ConvHalfCpltCallback (ADC_HandleTypeDef * hadc1);
void HAL_ADC_ConvCpltCallback (ADC_HandleTypeDef * hadc1);
void OscilloscopeInit(ADC_HandleTypeDef *hadc1);
void OscilloscopeUpdateEncoder(void);
void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc);
void AdcSetChannel(uint32_t channel);

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
	OscilloscopeDrawHUD();
}

void HAL_ADC_ConvHalfCpltCallback (ADC_HandleTypeDef *hadc1)
{
	if (current_state != OSC_WAIT_DATA)
	{
		return;
	}

	if (GetCurrentMode() == MODE_SPECTRUM)
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

	if (GetCurrentMode() == MODE_SPECTRUM)
	{
		spectrum_data_ready = 1;
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

		calculated_value = raw_value - zero_offset;
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
		//y = ((calculated_value * 15) >> 8); // !!!!
		// Draw new frame
		// DrawScreen(i, old_y[i], BLACK);
		// DrawScreen (i, y, GREEN);
		if (i % 32 == 0 || old_y[i] % 30 == 0)
		{
		    ILI9341_DrawPixel(i, old_y[i], ILI9341_GREY);
		}
		else
		{
		    ILI9341_DrawPixel(i, old_y[i], ILI9341_BLACK);
		}
		ILI9341_DrawPixel(i, y, ILI9341_GREEN);
		old_y[i] = y;

	}
	current_state = OSC_WAIT_DATA;
}

void OscilloscopeDrawGrid(void)
{
	for (int i = 0 ; i < 320; i += 32)
	{
		ILI9341_FillRectangle(i, 0, 1, 240, ILI9341_GREY);
	}

	for (int i = 0; i < 240; i += 30)
	{
		ILI9341_FillRectangle(0, i, 320, 1, ILI9341_GREY);
	}
}

void OscilloscopeUpdateEncoder(void)
{
    uint32_t current_tim_value = __HAL_TIM_GET_COUNTER(&htim4);

    if (current_tim_value == last_encoder_val)
    {
        return;
    }

    int16_t diff = (int16_t)(last_encoder_val - current_tim_value);
    last_encoder_val = current_tim_value;

    switch (scale_controls_mode)
    {
    case TIME:
    	if (diff > 0 && timebase_index < 5)
    	{
    		timebase_index++;
    		__HAL_TIM_SET_AUTORELOAD(&htim2, timebase_arr_values[timebase_index]);
    		__HAL_TIM_SET_COUNTER(&htim2, 0);
    	}
    	else if (diff < 0 && timebase_index > 0)
    	{
    		timebase_index--;
    		__HAL_TIM_SET_AUTORELOAD(&htim2, timebase_arr_values[timebase_index]);
    		__HAL_TIM_SET_COUNTER(&htim2, 0);
    	}
    	break;
    case VOLTS:
    	if (diff > 0 && volts_index < 3)
    	{
    		volts_index++;
    	}
    	else if (diff < 0 && volts_index > 0)
    	{
    		volts_index--;
    	}
    	break;
    case TRIGGER:
    	if (diff > 0)
    	{
    		trigger_level += 50;
    		if (trigger_level > 4095)
    		{
    			trigger_level = 4095;
    		}
    	}
    	else if (diff < 0)
    	{
    		trigger_level -= 50;
    		if (trigger_level < 0)
    		{
    			trigger_level = 0;
    		}
    	}
    	break;
    }
//
//    if (diff > 0)
//    {
//        if (scale_controls_mode == TIME && timebase_index < 5)
//        {
//            timebase_index++;
//            __HAL_TIM_SET_AUTORELOAD(&htim2, timebase_arr_values[timebase_index]);
//        }
//        else if (scale_controls_mode == VOLTS && volts_index < 3)
//        {
//        	volts_index++;
//        }
//        else if (scale_controls_mode == TRIGGER)
//        {
//        	trigger_level += 50;
//        	if (trigger_level > 4095)
//        	{
//        		trigger_level = 4095;
//        	}
//        }
//    }
//    else if (diff < 0)
//    {
//        if (scale_controls_mode == TIME && timebase_index > 0)
//        {
//            timebase_index--;
//            __HAL_TIM_SET_AUTORELOAD(&htim2, timebase_arr_values[timebase_index]);
//        }
//        else if (scale_controls_mode == VOLTS && volts_index > 0)
//        {
//        	volts_index--;
//        }
//        else if (scale_controls_mode == TRIGGER)
//        {
//        	trigger_level -= 50;
//        	if (trigger_level < 0)
//        	{
//        		trigger_level = 0;
//        	}
//        }
//    }
}

void OscilloscopeCalibrateZero()
{
	uint32_t temp_value = 0;
	for (uint16_t i = 0; i < 4096; i++)
	{
		temp_value += adc_buffer[i];
	}
	zero_offset = temp_value >> 12; //zero_offset /= 4096
}

void OscilloscopeToggleMode()
{
	scale_controls_mode = (scale_controls_mode + 1) % (TRIGGER + 1);
}

static void OscilloscopeTrigger()
{
	uint16_t start_index = (buffer_flag == 0) ? 1 : 2049;
	uint16_t end_index = start_index + 2047 - 320;

	for (int i = start_index; i < end_index; i++)
	{
		if (adc_buffer[i-1] < trigger_level && adc_buffer[i] >= trigger_level)
		{
			trigger_index = i;
			current_state = OSC_DRAW_SCREEN;
			break;
		}
	}

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

void AdcSetChannel(uint32_t channel)
{
	ADC_ChannelConfTypeDef sConfig = {0};

	HAL_ADC_Stop_DMA(&hadc1);

	sConfig.Channel = channel;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}

	HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buffer, 4096);
}

void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc)
{
	if (hadc->Instance == ADC1)
		{
			HAL_ADC_Stop_DMA(hadc);
			HAL_ADC_Start_DMA(hadc, (uint32_t*)adc_buffer, 4096);
		}
}

void OscilloscopeDrawHUD(void)
{
	char str_buff[32];
	uint16_t color;

	color = (scale_controls_mode == TIME) ? ILI9341_YELLOW : ILI9341_WHITE;
	sprintf(str_buff, "T:%s  ", timebase_strings[timebase_index]);
	ILI9341_WriteString(5, 5, str_buff, Font_7x10, color, ILI9341_BLACK);

	color = (scale_controls_mode == VOLTS) ? ILI9341_YELLOW : ILI9341_WHITE;
	sprintf(str_buff, "V:%s  ", volts_strings[volts_index]);
	ILI9341_WriteString(100, 5, str_buff, Font_7x10, color, ILI9341_BLACK);

	color = (scale_controls_mode == TRIGGER) ? ILI9341_YELLOW : ILI9341_WHITE;
	sprintf(str_buff, "Trg:%d   ", (int)trigger_level);
	ILI9341_WriteString(200, 5, str_buff, Font_7x10, color, ILI9341_BLACK);

	ILI9341_WriteString(200, 230, last_mode, Font_7x10, ILI9341_GREEN, ILI9341_BLACK);

	if (current_state == OSC_PAUSED)
	{
		ILI9341_WriteString(290, 5, "STOP", Font_7x10, ILI9341_RED, ILI9341_BLACK);
	}
	else
	{
		ILI9341_WriteString(290, 5, "RUN ", Font_7x10, ILI9341_GREEN, ILI9341_BLACK);
	}
}
