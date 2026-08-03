#include "buttons.h"

typedef enum
{
	MODE_OSCILLOSCOPE,
	MODE_SPECTRUM,
	MODE_SETTINGS
} Mode;

void MenuUpdate(void);

static void NextMode(void);
static void ChangeFocus(void);
static void RunStop(void);
static void ScrollSweep(void);
static void RunOscilloscope(void);
static void RunSpectrum(void);
static void RunSettings(void);
