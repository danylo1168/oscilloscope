#include "display.h"
#include "main.h"
#include "ili9341.h"
#include <stdint.h>
#include <stddef.h>

extern SPI_HandleTypeDef hspi2;

void *display = NULL;

void ScreenInit(void)
{
	ILI9341_Init();
	display = (void*)1;
}

void ScreenFill(uint16_t color)
{

	ILI9341_SetWindow(0, 0, GUI_WIDTH - 1, GUI_HEIGHT - 1);

	uint8_t data[2] = {color >> 8, color & 0xFF};

	HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_SET);

	for(uint32_t i = 0; i < (GUI_WIDTH * GUI_HEIGHT); i++)
	{
		HAL_SPI_Transmit(&hspi2, data, 2, HAL_MAX_DELAY);
	}
}

void ScreenDrawPixel(uint16_t x, uint16_t y, uint16_t color)
{
	ILI9341_WritePixel(x, y, color);
}

void ScreenDrawTestPattern(void)
{

	uint16_t colors[3] = {0xF800, 0x07E0, 0x001F};
	uint16_t width = GUI_WIDTH / 3;

	for(int c = 0; c < 3; c++)
	{

		ILI9341_SetWindow(c * width, 0, (c + 1) * width - 1, GUI_HEIGHT - 1);

		uint8_t data[2] = {colors[c] >> 8, colors[c] & 0xFF};

		HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_SET);

		for(uint32_t i = 0; i < (width * GUI_HEIGHT); i++)
		{
			HAL_SPI_Transmit(&hspi2, data, 2, HAL_MAX_DELAY);
		}
	}
}
