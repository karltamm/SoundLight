#include <screen.h>

#include <Arduino.h>
#include <stdlib.h>

/* PRIVATE CONSTANTS */
#define SCREEN_DATA_PIN  PIN7
#define SCREEN_RGB_ORDER GRB
#define SCREEN_TYPE      WS2812B

#define SCREEN_ROW_COUNT    16
#define SCREEN_COLUMN_COUNT 16
#define SCREEN_PIXEL_COUNT  256

#define SCREEN_MAX_VOLTS     5
#define SCREEN_MAX_MILLIAMPS 400

/* PRIVATE GLOBAL VARIABLES */
static uint8_t g_brightness;
static CRGB* g_pixels = NULL;

/* FUNCTIONS */
void init_screen(uint8_t brightness) {
  g_brightness = brightness;

  g_pixels = (CRGB*)malloc(sizeof(CRGB) * SCREEN_PIXEL_COUNT);
  if (g_pixels == NULL) {
    return;
  }

  FastLED.addLeds<SCREEN_TYPE, SCREEN_DATA_PIN, SCREEN_RGB_ORDER>(g_pixels, SCREEN_PIXEL_COUNT);
  FastLED.setMaxPowerInVoltsAndMilliamps(SCREEN_MAX_VOLTS, SCREEN_MAX_MILLIAMPS);
  FastLED.clear(true);
}

// TODO: stop screen (deallocate)

void set_pixel(uint8_t pixel_index, CRGB color) {
  if (pixel_index >= SCREEN_PIXEL_COUNT || g_pixels == NULL) {
    return;
  }

  g_pixels[pixel_index] = color;
}

uint8_t get_pixel_index(uint8_t row_index, uint8_t column_index) {
  if (row_index >= SCREEN_ROW_COUNT || column_index >= SCREEN_COLUMN_COUNT) {
    return;
  }

  /*
    Pixel indexes on the screen are in snake pattern.
    This limitation is set by the hardware.
    Example:
    0, 1, 2, 3
    7, 6, 5, 4
    ...
   */

  if (row_index % 2 != 0) {
    // Need to update passed column index,
    // because column indexes in odd rows are backwards.
    column_index = (SCREEN_COLUMN_COUNT - 1) - column_index;  // "-1" cause indexes start from 0
  }

  return (SCREEN_PIXEL_COUNT - 1) - (row_index * SCREEN_COLUMN_COUNT + column_index);
}
