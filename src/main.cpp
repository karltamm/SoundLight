#include <Arduino.h>
#include <colorWall.h>
#include <TM1637Display.h>

/* CONSTANTS */
/* Pins */
const uint8_t MIC_PIN = A1;
const uint8_t DISPLAY_PIN = 3;
const uint8_t CLK_PIN = 5;

/* Colors */
RGB YELLOW(245, 245, 66);
RGB RED(245, 84, 66);
RGB BLUE(66, 81, 245);
RGB GOLD(245, 179, 66);
RGB CRIMSON(220, 20, 60);
RGB INDIGO(63, 0, 255);

/* Gradients */
Gradient YELLOW_GRAD(GOLD, YELLOW);
Gradient RED_GRAD(CRIMSON, RED);
Gradient BLUE_GRAD(INDIGO, BLUE);

/* Sound */
const uint16_t MIN_VOL = 270; // Measured volume value can be from 0 to 1023. With small amp. gain, everything below 270 is noise.
const uint8_t LOCAL_MEASURING_INTERVAL = 100; // ms
const uint16_t PEAK_SOUND_RESET_INTERVAL = 5000; // ms

/* PROTOTYPES */
int getSoundVolume();

/* GLOBAL VARIABLES */
/* Components */
TM1637Display display(CLK_PIN, DISPLAY_PIN);

/* ColorWall */
ColorWall visible(BLUE_GRAD);
ColorWall buffer(YELLOW_GRAD);

/* Sound level */
unsigned long last_local_measuring_time;
unsigned long last_peak_reset_time;
uint16_t peak_sound;

/* MAIN */
void setup() {
    /* Serial monitor */
    Serial.begin(9600);

    /* Pins */
    pinMode(MIC_PIN, INPUT);

    /* Display */
    display.clear();
    display.setBrightness(7);

    /* ColorWall test */
    visible.print();
    for(uint8_t i = 0; i < 6; i++){
        visible.flowToRight();
        buffer.flowInto(&visible);
        buffer.flowToRight();
    }
    visible.print();
    Serial.print("test");
}

void loop() {
    /* Measure sound level */
    uint16_t local_max_sound = getSoundVolume();

    /* Update LED display */
    display.showNumberDec(8888);
    uint8_t LED_brightness = map(local_max_sound, MIN_VOL, peak_sound, 1, 7);
    display.setBrightness(LED_brightness);
}

/* FUNCTIONS */
int getSoundVolume(){
    uint16_t sample, local_max_sound = 0;

    /* Get sound volume fluctuations */
    while(millis() - last_local_measuring_time < LOCAL_MEASURING_INTERVAL){
        sample = analogRead(MIC_PIN);
        sample = sample < MIN_VOL ? MIN_VOL : sample; // Exclude noise

        if(sample > local_max_sound){
            local_max_sound = sample;
        }

        if(sample > peak_sound){
            peak_sound = sample;
        }
    }
    last_local_measuring_time = millis();

    /* Make sure that peak sound is relevant to current music/situation  */
    if(millis() - last_peak_reset_time >= PEAK_SOUND_RESET_INTERVAL){
        peak_sound = local_max_sound;
        last_peak_reset_time = millis();
    }

    return local_max_sound;
}