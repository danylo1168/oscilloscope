#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>

#define RED 0xF800
#define GREEN 0x07E0
#define BLACK 0x0000

void ScreenInit(void);
void ScreenFill(uint16_t color);
void ScreenDrawPixel(uint16_t x, uint16_t y, uint16_t color);
void ScreenDrawTestPattern(void);

#endif /* DISPLAY_H */
