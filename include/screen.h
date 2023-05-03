#ifndef SCREEN_H
#define SCREEN_H

#include <FastLED.h>
#include <stdint.h>

/* PUBLIC CONSTANTS */
#define DEFAULT_SCREEN_BRIGHTNESS 255

#define SCREEN_ROW_COUNT    16
#define SCREEN_COLUMN_COUNT 16
#define SCREEN_PIXEL_COUNT  256

/* PUBLIC PROTOTYPES */
void init_screen(uint8_t brightness);
void set_pixel(uint8_t pixel_index, CRGB color);
uint8_t get_pixel_index(uint8_t row_index, uint8_t column_index);
uint8_t get_screen_brightness();

#endif