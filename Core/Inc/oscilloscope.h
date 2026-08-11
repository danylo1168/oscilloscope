#ifndef OSCILLOSCOPE_H
#define OSCILLOSCOPE_H

#include "main.h"

typedef enum
{
	OSC_PAUSED,
	OSC_WAIT_DATA,
	OSC_FIND_TRIGGER,
	OSC_DRAW_SCREEN
} State;

typedef enum
{
	TIME,
	VOLTS,
	TRIGGER
} ScaleControls;

void OscilloscopeInit(ADC_HandleTypeDef *hadc1);
void OscilloscopeUpdate(void);
void OscilloscopeToggleRunStop(void);
void OscilloscopeToggleMode(void);
void OscilloscopeUpdateEncoder(void);
void AdcSetChannel(uint32_t channel);

#endif
