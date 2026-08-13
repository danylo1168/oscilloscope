#include "menu.h"
#include "buttons.h"
#include "oscilloscope.h"
#include "spectrum.h"
#include <stdio.h>

static Mode current_mode = MODE_OSCILLOSCOPE;

char last_mode[20] = "BTN: IDLE";

Events current_event;

Mode GetCurrentMode(void);
void NextMode(void);
void MenuUpdate(void);

void NextMode()
{
	current_mode = (current_mode + 1) % (MODE_SETTINGS + 1);

	if (current_mode == MODE_OSCILLOSCOPE)
	{
		AdcSetChannel(ADC_CHANNEL_6);
	}
	else if (current_mode == MODE_SPECTRUM)
	{
		AdcSetChannel(ADC_CHANNEL_7);
	}
}

void MenuUpdate()
{
	for (int i = 0; i < 4; i++)
	{
		current_event = ButtonGetEvent(i);
		if (current_event == BTN_CLICK)
		{
			sprintf(last_mode, "BUTTON INDEX: %d", (int)i);
			switch (i)
			{
			case 0:

				OscilloscopeToggleMode();
				break;
			case 1:
				NextMode();
				break;
			case 2:

				break;
			case 3:
				if (current_mode == MODE_OSCILLOSCOPE)
				{
					OscilloscopeToggleRunStop();
				}
				else if (current_mode == MODE_SPECTRUM)
				{
					//SpectrumToggleRunStop();
				}
				break;
			}
		}
	}

	switch (current_mode)
			{
			case MODE_OSCILLOSCOPE:
				OscilloscopeUpdate();
				break;
			case MODE_SPECTRUM:
				RunSpectrum();
				break;
			case MODE_SETTINGS:
				//RunSettings();
				break;
			}
}

Mode GetCurrentMode(void)
{
	return current_mode;
}

