#include "buttons.h"

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
			btn_list[i].press_time = 0;
			btn_list[i].is_pressed = 1;
			break;
		}
	}
}
