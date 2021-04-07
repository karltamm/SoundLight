#include <Arduino.h>
#include <colorWall.h>
#include <TM1637Display.h>

/* CONSTANTS */
/* Pins */
const uint8_t MIC_PIN = A1;
const uint8_t DISPLAY_PIN = 3;
const uint8_t CLK_PIN = 5;

/* Colors */
const RGB YELLOW(245, 245, 66);
const RGB RED(245, 84, 66);
const RGB BLUE(66, 81, 245);
const RGB GOLD(245, 179, 66);
const RGB CRIMSON(220, 20, 60);
const RGB INDIGO(63, 0, 255);

/* Gradients */
const Gradient YELLOW_GRAD(GOLD, YELLOW);
const Gradient RED_GRAD(CRIMSON, RED);
const Gradient BLUE_GRAD(INDIGO, BLUE);

/* Sound */
const uint16_t MIN_VOL = 270; // Measured volume value can be from 0 to 1023. Consider everything below MIN_VOL as noise.
const uint8_t LOCAL_MEASURING_INTERVAL = 50; // ms
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
    // visible.print();
    // for(uint8_t i = 0; i < 6; i++){
    //     visible.flowToRight();
    //     buffer.flowInto(&visible);
    //     buffer.flowToRight();
    // }
    // visible.print();
}

void loop() {
    /* Show sound volume */
    display.showNumberDec(8888);
    uint8_t LED_brightness = map(getSoundVolume(), MIN_VOL, peak_sound, 1, 7);
    display.setBrightness(LED_brightness);
}

/* FUNCTIONS */
int getSoundVolume(){
    uint16_t sample, local_max_sound = 0;

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
    
    //Serial.println(local_max_sound);
    return local_max_sound;
}