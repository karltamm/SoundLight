#include <animations.h>

/* CONSTANTS */
/* Pins */
const uint8_t MIC_PIN = A1;
const uint8_t LED_PIN = 3;

/* LED display */
const uint8_t NUM_ROWS = 16;
const uint8_t NUM_COLS = 16;

const uint8_t MIN_BRIGHTNESS = 15;
const uint8_t MAX_BRIGHTNESS = 255;

/* GLOBAL VARIABLES */
/* LED Display */
Screen screen(NUM_COLS, NUM_ROWS);

/* Microphone */
Microphone mic(MIC_PIN);

/* Animation */
Rabbithole rabbithole(&screen, &mic);
Triangles triangles(&screen, &mic);

/* Sound level */
unsigned long last_peak_reset_time;
uint16_t peak_sound;

/* MAIN */
void setup() {
    /* Serial monitor */
    Serial.begin(9600);

    /* Init FastLED */
    FastLED.addLeds<WS2812B, LED_PIN, GRB>(screen.leds, screen.NUM_LEDS);
    FastLED.setMaxPowerInVoltsAndMilliamps(5, 400);
    FastLED.clear(true);

    /* Rabbithole animation */
    rabbithole.init();
}

void loop() {
    //rabbithole.run();

    triangles.animate();
}