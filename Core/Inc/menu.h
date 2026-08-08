#ifndef MENU_H
#define MENU_H

typedef enum
{
	MODE_OSCILLOSCOPE,
	MODE_SPECTRUM,
	MODE_SETTINGS
} Mode;

void MenuUpdate(void);

#endif /* MENU_H */
