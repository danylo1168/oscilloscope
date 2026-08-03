
typedef enum
{
	OSC_PAUSED,
	OSC_WAIT_DATA,
	OSC_FIND_TRIGGER,
	OSC_DRAW_SCREEN
} State;

void OscilloscopeUpdate(void);
void OscilloscopeToggleRunStop(void);
void OsclilloscopeTrigger(void);
void OscilloscopeSendData(void);
