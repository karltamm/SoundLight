#include <microphone.h>

#include <Arduino.h>

/* PRIVATE CONSTANTS */
#define MICROPHONE_PIN A2

#define MEASURE_INTERVAL_MS          20
#define AVG_VOLUME_RESET_INTERVAL_MS 10000

#define VOLUME_BUMP_THRESHOLD 20
#define NOISE_LEVEL_THRESHOLD 270
#define MAX_VOLUME            1023

/* PRIVATE TYPES */
typedef unsigned long time_t;

/* PRIVATE GLOBAL VARIABLES */
static uint16_t g_avg_peak_volume = 0;

/* PRIVATE PROTOTYPES */
uint16_t get_peak_volume_change();
uint16_t get_current_peak_volume();

/* FUNCTIONS */
void init_microphone() {
  pinMode(MICROPHONE_PIN, INPUT);
}

bool detect_volume_bump() {
  return get_peak_volume_change() > VOLUME_BUMP_THRESHOLD;
}

uint16_t get_peak_volume_change() {
  static uint16_t last_peak_volume = 0;

  uint16_t peak_volume_change = 0;
  uint16_t current_peak_volume = get_current_peak_volume();

  if (current_peak_volume > g_avg_peak_volume) {
    peak_volume_change = abs((short)(current_peak_volume - last_peak_volume));
  }

  last_peak_volume = current_peak_volume;

  return peak_volume_change;
}

uint16_t get_current_peak_volume() {
  static uint32_t avg_peak_volume_measurements_count = 0;
  static time_t last_avg_peak_volume_rst_time = 0;

  uint16_t current_peak_volume = 0;

  time_t start_time = (time_t)millis();
  while (millis() - start_time < MEASURE_INTERVAL_MS) {
    uint16_t volume_sample = analogRead(MICROPHONE_PIN);
    volume_sample = volume_sample < NOISE_LEVEL_THRESHOLD ? 0 : volume_sample;

    if (volume_sample > MAX_VOLUME || volume_sample < current_peak_volume) {
      continue;
    }

    current_peak_volume = volume_sample;
  }

  if (millis() - last_avg_peak_volume_rst_time >= AVG_VOLUME_RESET_INTERVAL_MS) {
    last_avg_peak_volume_rst_time = (time_t)millis();
    g_avg_peak_volume = 0;
    avg_peak_volume_measurements_count = 0;
  }

  uint32_t avg_buffer = g_avg_peak_volume * avg_peak_volume_measurements_count + current_peak_volume;
  g_avg_peak_volume = avg_buffer / ++avg_peak_volume_measurements_count;

  return current_peak_volume;
}
