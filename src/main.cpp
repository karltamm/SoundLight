#include <animations.h>

/* CONSTANTS */
/* Pins */
const uint8_t MIC_PIN = A1;
const uint8_t LED_PIN = 3;

/* Sound */
const uint16_t MIN_VOL = 300; // Measured volume value can be from 0 to 1023. Consider everything below MIN_VOL as noise.

const uint8_t LOCAL_MEASURING_INTERVAL = 50; // ms
const uint16_t PEAK_SOUND_RESET_INTERVAL = 5000; // ms

/* LED display */
const uint8_t NUM_ROWS = 16;
const uint8_t NUM_COLS = 16;

const uint8_t MIN_BRIGHTNESS = 15;
const uint8_t MAX_BRIGHTNESS = 255;

/* PROTOTYPES */
uint16_t getSoundVolume();

/* GLOBAL VARIABLES */
/* LED Display */
Screen screen(NUM_COLS, NUM_ROWS);

/* Animation */
Rabbithole animation(&screen);

/* Sound level */
unsigned long last_peak_reset_time;
uint16_t peak_sound;

/* MAIN */
void setup() {
    /* Serial monitor */
    Serial.begin(9600);

    /* Pins */
    pinMode(MIC_PIN, INPUT);

    /* Init FastLED */
    FastLED.addLeds<WS2812B, LED_PIN, GRB>(screen.leds, screen.NUM_LEDS);
    FastLED.setMaxPowerInVoltsAndMilliamps(5, 400);
    FastLED.clear(true);

    /* Start the animation */
    animation.init();
}

void loop() {
    uint16_t vol = getSoundVolume();

    screen.brightness = map(vol, MIN_VOL, peak_sound, MIN_BRIGHTNESS, MAX_BRIGHTNESS);
    animation.run();
}

/* FUNCTIONS */
uint16_t getSoundVolume(){
    uint16_t sample;
    uint16_t local_max_sound = 0;

    /* Get max volume during interval */
    unsigned long start_time = millis();

    while(millis() - start_time < LOCAL_MEASURING_INTERVAL){
        sample = analogRead(MIC_PIN);
        sample = sample < MIN_VOL ? MIN_VOL : sample; // Exclude noise

        if(sample > local_max_sound){
            local_max_sound = sample;
        }

        if(sample > peak_sound){
            peak_sound = sample;
        }
    }

    /* Make sure that peak sound is relevant to current music/situation */
    /* Therefore, update peak volume at least after some interval  */
    if(millis() - last_peak_reset_time >= PEAK_SOUND_RESET_INTERVAL){
        peak_sound = local_max_sound;
        last_peak_reset_time = millis();
    }
    
    return local_max_sound;
}