#ifndef MENU_H
#define MENU_H

typedef enum
{
	MODE_OSCILLOSCOPE,
	MODE_SPECTRUM,
	MODE_SETTINGS
} Mode;

Mode GetCurrentMode(void);
void NextMode(void);
void MenuUpdate(void);

#endif /* MENU_H */
