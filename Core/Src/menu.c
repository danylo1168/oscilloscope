#include "menu.h"
#include "buttons.h"
#include "oscilloscope.h"
#include "spectrum.h"

static Mode current_mode = MODE_OSCILLOSCOPE;

static void NextMode()
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
		if (ButtonGetEvent(i) == BTN_CLICK)
		{
			switch (i)
			{
			case BTN_MODE:
				NextMode();
				break;
			case BTN_FOCUS:
				OscilloscopeToggleMode();
				break;
			case BTN_RUN_STOP:
				if (current_mode == MODE_OSCILLOSCOPE)
				{
					OscilloscopeToggleRunStop();
				}
				else if (current_mode == MODE_SPECTRUM)
				{
					//SpectrumToggleRunStop();
				}
				break;
			case BTN_SCROLL:
				//Scroll();
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
