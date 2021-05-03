#include <Arduino.h>
#include <FastLED.h>

/* CONSTANTS */
/* Pins */
const uint8_t MIC_PIN = A1;
const uint8_t LED_PIN = 3;

/* Sound */
const uint16_t MIN_VOL = 270; // Measured volume value can be from 0 to 1023. Consider everything below MIN_VOL as noise.
const uint8_t LOCAL_MEASURING_INTERVAL = 50; // ms
const uint16_t PEAK_SOUND_RESET_INTERVAL = 5000; // ms

/* LED display */
const uint8_t NUM_ROWS = 16;
const uint8_t NUM_COLS = 16;
const uint16_t NUM_LEDS = NUM_ROWS * NUM_COLS;

/* Hue */
HSVHue HUES[] = {HUE_PURPLE, HUE_YELLOW, HUE_BLUE, HUE_RED, HUE_GREEN, HUE_PINK};
uint8_t NUM_HUES = 6;

/* PROTOTYPES */
int getSoundVolume();
uint8_t ledIndex(uint8_t, uint8_t);
CHSV getHSV(HSVHue);
void setSourceLed();
void setMaxSteps();
void growAnimation();
void paintAllSameColor(HSVHue);
void runAnimation();
CHSV getGrowingHue();

/* GLOBAL VARIABLES */
/* LED Display */
CRGB leds[NUM_LEDS];
uint8_t brightness = 50;

/* Animation */
uint8_t source_xy[2]; // From which LED the animation starts to grow

uint8_t step = 1; // How many times has animation grown?
uint8_t max_steps; // How many steps can animation grow?

uint8_t main_hue = 0; // Current index

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
    FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setMaxPowerInVoltsAndMilliamps(5, 400);
    FastLED.clear(true);
    FastLED.show();

    /* Start the animation */
    randomSeed(analogRead(A0)); // Make random more random
    paintAllSameColor(HUES[main_hue]);
}

void loop() {
    growAnimation();
    delay(100);
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

uint8_t ledIndex(uint8_t col, uint8_t row){
    /* LEDs are placed on the display in snake pattern (like "S").

    Example:
    0, 1, 2, 3
    7, 6, 5, 4
    ...
    
    */

   /* Indexes in odd rows are backwards. Find correct display column position */
   if(row % 2 == 1){
       col = (NUM_COLS - 1) - col; // reverse the numbering; "-1" becasue numbering starts from 0
   }

    /* Calculate index */
    return (NUM_LEDS - 1) - (row * NUM_COLS + col); // start index > end; "row * NUM_COLS" => full rows
}

CHSV getHSV(HSVHue hue){
    return CHSV(hue, 255, brightness);
}

void setSourceLed(){
    /* Generate random X and Y coordinates according to the LED matrix */
    source_xy[0] = random() % (NUM_COLS - 1); // x
    source_xy[1] = random() % (NUM_ROWS - 1); // y

    leds[ledIndex(source_xy[0], source_xy[1])] = getGrowingHue();
    FastLED.show();

    setMaxSteps();
}

void setMaxSteps(){
    uint8_t max_x_steps; // How many steps can animation grow in X direction?
    if(source_xy[0] < NUM_COLS / 2){ // If source led is on the left side of display
        max_x_steps = (NUM_COLS - 1) - source_xy[0]; // Distance to the right must be greater than to the left
    }else{
        max_x_steps = source_xy[0];
    }

    uint8_t max_y_steps; // How many steps can animation grow in Y direction?
    if(source_xy[1] < NUM_ROWS / 2){ // If source led is on the top side of display
        max_y_steps = (NUM_ROWS - 1) - source_xy[1]; // Distance to the bottom must be greater than to the top
    }else{
        max_y_steps = source_xy[1];
    }

    max_steps = max_x_steps > max_y_steps ? max_x_steps : max_y_steps;
}

void growAnimation(){
    if(step <= max_steps){
        /* Every step, the animation "diameter" grows larger. It covers more rows and columns */
        for(int8_t col = source_xy[0] - step; col <= source_xy[0] + step; col++){
            for(int8_t row = source_xy[1] - step; row <= source_xy[1] + step; row++){
                if(col >= 0 && col < NUM_COLS && row >= 0 && row < NUM_ROWS){ // Only update actual display area, because animation technically also grows outside 
                    leds[ledIndex(col, row)] = getGrowingHue();
                }
            }
        }

        FastLED.show();
        step++;
    }else{
        runAnimation();
    }

}

void paintAllSameColor(HSVHue hue){
    for(uint8_t col = 0; col < NUM_COLS; col++){
        for(uint8_t row = 0; row < NUM_ROWS; row++){
            leds[ledIndex(col, row)] = getHSV(hue);
        }
    }
    FastLED.show();
}

void runAnimation(){
    step = 1;

    main_hue++;
    main_hue %= NUM_HUES; // Avoid overflow

    setSourceLed();
}

CHSV getGrowingHue(){
    return getHSV(HUES[(main_hue + 1) % NUM_HUES]);
}