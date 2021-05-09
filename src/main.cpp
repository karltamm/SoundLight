#include <animations.h>

/* CONSTANTS */
/* Pins */
const uint8_t MIC_PIN = A1;
const uint8_t LED_PIN = 3;

/* LED display */
const uint8_t NUM_ROWS = 16;
const uint8_t NUM_COLS = 16;

/* GLOBAL VARIABLES */
/* LED Display */
Screen screen(NUM_COLS, NUM_ROWS);

/* Microphone */
Microphone mic(MIC_PIN);

/* Animation */
//Rabbithole rabbithole(&screen, &mic);
Plasma plasma(&screen, &mic);

/* MAIN */
void setup() {
    /* Serial monitor */
    Serial.begin(9600);

    /* Init FastLED */
    FastLED.addLeds<WS2812B, LED_PIN, GRB>(screen.leds, screen.NUM_LEDS);
    FastLED.setMaxPowerInVoltsAndMilliamps(5, 400);
    FastLED.clear(true);

    /* Animations */
    //rabbithole.init();
}

void loop() {
    plasma.soundReactivePlasma();
}