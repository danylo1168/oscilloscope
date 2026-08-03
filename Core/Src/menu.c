#include "menu.h"

static Mode current_mode = MODE_OSCILLOSCOPE;

static void NextMode();
static void ChangeFocus();
static void RunStop();
static void ScrollSweep();
static void RunOscilloscope();
static void RunSpectrum();
static void RunSettings();

void MenuUpdate()
{
	for (int i = 0; i < 4; i++)
	{
		if (ButtonGetEvent(i) == BTN_CLICK)
		{
			switch(i)
			{
			case BTN_MODE:
				NextMode();
				break;
			case BTN_FOCUS:
				ChangeFocus();
				break;
			case BTN_RUN_STOP:
				RunStop();
				break;
			case BTN_SCROLL:
				ScrollSweep();
				break;
			}
		}
	}

	switch (current_mode)
	{
	case MODE_OSCILLOSCOPE:
		RunOscilloscope();
		break;

	case MODE_SPECTRUM:
		RunSpectrum();
		break;

	case MODE_SETTINGS:
		RunSettings();
		break;
	}
}

static void NextMode()
{
	current_mode = (current_mode + 1) % (MODE_SETTINGS + 1);
}

static void ChangeFocus()
{

}

static void RunStop()
{

}

static void ScrollSweep()
{

}

static void RunOscilloscope()
{

}

static void RunSpectrum()
{

}

static void RunSettings()
{

}
