#include "buttons.h"
#include "main.h"

extern TIM_HandleTypeDef htim10;

typedef struct
{
	GPIO_TypeDef* port;
	uint16_t pin;
	Events event;
	uint32_t press_time;
	uint8_t is_pressed;
} ButtonState;

static ButtonState btn_list[4] =
{
		{GPIOB, GPIO_PIN_8, BTN_IDLE, 0, 0},
		{GPIOB, GPIO_PIN_5, BTN_IDLE, 0, 0},
		{GPIOB, GPIO_PIN_4, BTN_IDLE, 0, 0},
		{GPIOB, GPIO_PIN_3, BTN_IDLE, 0, 0}
};

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	for (int i = 0; i < 4; i++)
	{
		if (GPIO_Pin == btn_list[i].pin)
		{
			HAL_TIM_Base_Start_IT(&htim10);
			btn_list[i].press_time = 0;
			btn_list[i].is_pressed = 1;
			break;
		}
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance != TIM10)
	{
		return;
	}

	uint8_t any_active_button = 0;

	for (int i = 0; i < 4; i++)
	{
		if (btn_list[i].is_pressed  == 0)
		{
			continue;
		}

		if (HAL_GPIO_ReadPin(btn_list[i].port, btn_list[i].pin) == GPIO_PIN_SET)
		{
			if (btn_list[i].press_time > 5 && btn_list[i].press_time < 500)
			{
				btn_list[i].event = BTN_CLICK;
			}
			btn_list[i].is_pressed = 0;
			continue;
		}

		any_active_button = 1;
		btn_list[i].press_time += 1;

		if (btn_list[i].press_time >= 500)
		{
			btn_list[i].event = BTN_LONG_PRESS;
			btn_list[i].is_pressed = 0;
		}
	}

	if (any_active_button == 0)
	{
		HAL_TIM_Base_Stop_IT(&htim10);
	}
}

Events ButtonGetEvent(uint8_t btn_index)
{
	Events temp = btn_list[btn_index].event;
	btn_list[btn_index].event = BTN_IDLE;
	return temp;
}
