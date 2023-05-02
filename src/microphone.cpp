#include <microphone.h>

#include <Arduino.h>

/* PRIVATE CONSTANTS */
#define MICROPHONE_PIN               A2
#define MEASURE_INTERVAL_MS          30
#define NOISE_LEVEL_THRESHOLD        270
#define VOLUME_BUMP_THRESHOLD        30
#define VOLUME_MAX_VALUE             1023
#define AVG_VOLUME_RESET_INTERVAL_MS 60000

/* PRIVATE GLOBAL VARIABLES */