#include <microphone.h>

#include <Arduino.h>

/* PRIVATE CONSTANTS */
#define MICROPHONE_PIN A2

#define MEASURE_INTERVAL_MS          30
#define AVG_VOLUME_RESET_INTERVAL_MS 60000

#define VOLUME_BUMP_THRESHOLD 30
#define NOISE_LEVEL_THRESHOLD 270
#define MAX_VOLUME            1023

/* PRIVATE TYPES */
typedef unsigned long time_t;

/* PRIVATE GLOBAL VARIABLES */
static uint16_t g_avg_volume = 0;
static uint32_t g_avg_volume_measurements_count = 0;
static uint16_t g_last_peak_volume = 0;
static time_t g_last_avg_volume_rst_time = 0;

/* PRIVATE PROTOTYPES */
uint16_t get_peak_volume_change();
uint16_t get_current_peak_volume();
uint16_t measure_volume();

/* FUNCTIONS */
void init_microphone() {
  pinMode(MICROPHONE_PIN, INPUT);
}

bool detect_volume_bump() {
  return get_peak_volume_change() > VOLUME_BUMP_THRESHOLD;
}

uint16_t get_peak_volume_change() {
  uint16_t peak_volume_change = 0;
  uint16_t current_peak_volume = get_current_peak_volume();

  if (current_peak_volume > g_avg_volume) {
    peak_volume_change = abs(current_peak_volume - g_last_peak_volume);
  }
  g_last_peak_volume = current_peak_volume;

  return peak_volume_change;
}

uint16_t get_current_peak_volume() {
  uint16_t volume_sample;
  uint16_t current_peak_volume = 0;

  time_t start_time = (time_t)millis();
  while (millis() - start_time < MEASURE_INTERVAL_MS) {
    volume_sample = analogRead(MICROPHONE_PIN);
    volume_sample = volume_sample < NOISE_LEVEL_THRESHOLD ? 0 : volume_sample;

    if (volume_sample > MAX_VOLUME || volume_sample < current_peak_volume) {
      continue;
    }

    current_peak_volume = volume_sample;
  }
}

uint16_t measure_volume() {
  uint16_t volume = analogRead(MICROPHONE_PIN);
  volume = volume < NOISE_LEVEL_THRESHOLD ? 0 : volume;

  g_avg_volume = (g_avg_volume + volume) / ++g_avg_volume_measurements_count;

  if (millis() - g_last_avg_volume_rst_time >= AVG_VOLUME_RESET_INTERVAL_MS) {
    g_last_avg_volume_rst_time = (time_t)millis();
    g_avg_volume = 0;
  }

  return volume;
}
