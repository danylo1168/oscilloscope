#ifndef INC_BUTTONS_H_
#define INC_BUTTONS_H_

#include "main.h"

typedef enum
{
	BTN_MODE,
	BTN_FOCUS,
	BTN_RUN_STOP,
	BTN_SCROLL
} Buttons;

typedef enum
{
	BTN_IDLE,
	BTN_CLICK,
	BTN_LONG_PRESS
} Events;

Events ButtonGetEvent(uint8_t btn_index);

#endif /* INC_BUTTONS_H_ */
