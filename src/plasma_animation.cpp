#include <plasma_animation.h>

#include <microphone.h>
#include <screen.h>

/* PRIVATE CONSTANTS */
#define SIMPLEX_MIN 50
#define SIMPLEX_MAX 190

#define PLASMA_BLOB_SCALE      25
#define NOISE_DEPTH_STEP_DELTA 10  // Noise algorithm z-coordinate change
#define MAX_NOISE_DEPTH        65000
#define MIN_NOISE_DEPTH        50

static const CRGB PINK_COLOR = CHSV(HUE_PINK, 255, 255);
static const CRGB BLUE_COLOR = CHSV(HUE_BLUE, 255, 255);
static const CRGB YELLOW_COLOR = CHSV(HUE_YELLOW, 255, 255);

static const CRGBPalette16 PINK_BLUE_PALETTE = CRGBPalette16(PINK_COLOR,
                                                             PINK_COLOR,
                                                             PINK_COLOR,
                                                             PINK_COLOR,
                                                             BLUE_COLOR,
                                                             BLUE_COLOR,
                                                             BLUE_COLOR,
                                                             BLUE_COLOR,
                                                             PINK_COLOR,
                                                             PINK_COLOR,
                                                             PINK_COLOR,
                                                             PINK_COLOR,
                                                             BLUE_COLOR,
                                                             BLUE_COLOR,
                                                             BLUE_COLOR,
                                                             BLUE_COLOR);

static const CRGBPalette16 PINK_YELLOW_PALETTE = CRGBPalette16(PINK_COLOR,
                                                               PINK_COLOR,
                                                               PINK_COLOR,
                                                               PINK_COLOR,
                                                               YELLOW_COLOR,
                                                               YELLOW_COLOR,
                                                               YELLOW_COLOR,
                                                               YELLOW_COLOR,
                                                               PINK_COLOR,
                                                               PINK_COLOR,
                                                               PINK_COLOR,
                                                               PINK_COLOR,
                                                               YELLOW_COLOR,
                                                               YELLOW_COLOR,
                                                               YELLOW_COLOR,
                                                               YELLOW_COLOR);

/* PRIVATE GLOBAL VARIABLES */
static uint16_t g_noise_depth = MIN_NOISE_DEPTH;  // z-coordinate
static int8_t g_noise_depth_step = NOISE_DEPTH_STEP_DELTA;

/* PRIVATE PROTOTYPES */
static void set_animation(CRGBPalette16 palette);

/* FUNCTIONS */
void update_plasma_animation() {
  if (detect_volume_bump()) {
    set_animation(PINK_YELLOW_PALETTE);
  } else {
    set_animation(PINK_BLUE_PALETTE);
  }
}

static void set_animation(CRGBPalette16 palette) {
  for (uint16_t x = 0; x < SCREEN_COLUMN_COUNT; x++) {
    for (uint16_t y = 0; y < SCREEN_ROW_COUNT; y++) {
      // Generating pixel hue based on Simplex algorithm
      uint8_t noise = inoise8(x * PLASMA_BLOB_SCALE,
                              y * PLASMA_BLOB_SCALE,
                              g_noise_depth);
      uint8_t hue_index = map(noise, SIMPLEX_MIN, SIMPLEX_MAX, 0, 255);
      set_pixel(get_pixel_index(x, y), ColorFromPalette(palette,
                                                        hue_index,
                                                        get_screen_brightness()));
    }
  }

  g_noise_depth += g_noise_depth_step;

  if (g_noise_depth > MAX_NOISE_DEPTH) {
    g_noise_depth_step = -1 * NOISE_DEPTH_STEP_DELTA;
  } else if (g_noise_depth < MIN_NOISE_DEPTH) {
    g_noise_depth_step = NOISE_DEPTH_STEP_DELTA;
  }

  FastLED.show();
}