#include <Arduino.h>
#include <FastLED.h>

/* CONSTANTS */
/* Microphone */
const uint8_t LOCAL_MEASURING_INTERVAL = 30; // ms
const uint16_t MIC_NOISE_LEVEL = 270; // Measured volume value can be from 0 to 1023. Consider everything below MIN_VOL as noise.
const uint8_t MIN_VOL_BUMP = 30;
const uint16_t VOL_MAX_VALUE = 1023;
const uint16_t AVERAGE_RESET_TIMER = 60000; // 1 minute in milliseconds

/* Screen */
const uint8_t DEFAULT_BRIGHTNESS = 200;

/* CLASSES */
class Screen{
    public:
        uint8_t NUM_ROWS;
        uint8_t NUM_COLS;
        uint16_t NUM_LEDS;

        uint8_t brightness = DEFAULT_BRIGHTNESS;
        CRGB* leds;

        Screen(uint8_t num_cols, uint8_t num_rows){
            /* Define screen size */
            NUM_ROWS = num_rows;
            NUM_COLS = num_cols;
            NUM_LEDS = num_rows * num_cols;

            /* Create LEDs array */
            leds = (CRGB*)malloc(sizeof(CRGB) * NUM_LEDS);
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

        CHSV getHSV(HSVHue hue, uint8_t lightness = 0){
            if(lightness == 0)
                lightness = brightness;

            return CHSV(hue, 255, lightness);
        }

        void paintAllSameColor(HSVHue hue){
            for(uint8_t col = 0; col < NUM_COLS; col++){
                for(uint8_t row = 0; row < NUM_ROWS; row++){
                    setLed(col, row, hue);
                }
            }
            FastLED.show();
        }

        void setLed(uint8_t col, uint8_t row, HSVHue hue){
            leds[ledIndex(col, row)] = getHSV(hue);
        }

        void setLed(uint8_t col, uint8_t row, CHSV color){
            leds[ledIndex(col, row)] = color;
        }

        void setLed(uint8_t col, uint8_t row, CRGB color){
            leds[ledIndex(col, row)] = color;
        }
};

class Microphone{
    private:
        uint8_t MIC_PIN; // Input pin

        uint16_t average_volume = 0;
        uint16_t last_peak = 0;
        uint16_t average_bump = 0;
        long unsigned last_average_reset = 0;

    public:
        Microphone(uint8_t pin){
            MIC_PIN = pin;
            pinMode(MIC_PIN, INPUT);
        }

        uint16_t getCurrentPeak(){
            uint16_t sample;
            uint16_t current_peak = 0;

            /* Measure volume */
            unsigned long start_time = millis();
            while(millis() - start_time < LOCAL_MEASURING_INTERVAL){
                sample = analogRead(MIC_PIN);
                sample = sample < MIC_NOISE_LEVEL ? 0 : sample;

                /* Filter */
                if(sample <= VOL_MAX_VALUE){ // Only accept normal values
                    if(sample > current_peak){
                        current_peak = sample;
                    }
                }
            }

            /* Update average peak volume */
            average_volume = (average_volume + current_peak) / 2;

            /* Reset the average volume from time to time to reflect the current situation */
            if(millis() - last_average_reset >= AVERAGE_RESET_TIMER){
                last_average_reset = millis();
                average_volume = 0;
            }

            return current_peak;
        }

        uint16_t getPeakChange(){
            uint16_t current_peak = getCurrentPeak();
            uint16_t peak_delta = 0; // How much current peak differs from last one

            if(current_peak > average_volume){ // Low volume doesn't count
                peak_delta = abs(current_peak - last_peak);
            }

            last_peak = current_peak;

            return peak_delta;
        }

        uint8_t isVolumeBump(){
            uint8_t is_bump = 0; // 1 = bump; 0 = no bump

            /* Find if there was any volume bump */
            if(getPeakChange() > MIN_VOL_BUMP){
                is_bump = 1;
            }

            return is_bump;
        }
};