#include <animations.h>

/* CONSTANTS */
/* Pins */
const uint8_t MIC_PIN = A1;
const uint8_t LED_PIN = 3;

/* LED display */
const uint8_t NUM_ROWS = 16;
const uint8_t NUM_COLS = 16;

/* GLOBAL VARIABLES */
/* External devices */
Screen screen(NUM_COLS, NUM_ROWS);
Microphone mic(MIC_PIN);

/* Animation */
Plasma plasma(&screen, &mic);

/* MAIN */
void setup() {
    /* Serial monitor */
    Serial.begin(9600);

    /* Init FastLED */
    FastLED.addLeds<WS2812B, LED_PIN, GRB>(screen.leds, screen.NUM_LEDS);
    FastLED.setMaxPowerInVoltsAndMilliamps(5, 2000);
    FastLED.clear(true);
}

void loop() {
    plasma.soundReactivePlasma();
}