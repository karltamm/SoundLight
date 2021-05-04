#include <Arduino.h>
#include <FastLED.h>

/* CONSTANTS */
/* Hue */
HSVHue HUES[] = {HUE_PURPLE, HUE_YELLOW, HUE_BLUE, HUE_RED, HUE_GREEN, HUE_PINK};
uint8_t NUM_HUES = 6;

/* CLASSES */
class Screen{
    public:
        uint8_t NUM_ROWS;
        uint8_t NUM_COLS;
        uint16_t NUM_LEDS;

        uint8_t brightness;
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

        CHSV getHSV(HSVHue hue){
            return CHSV(hue, 255, brightness);
        }

        void paintAllSameColor(HSVHue hue){
            for(uint8_t col = 0; col < NUM_COLS; col++){
                for(uint8_t row = 0; row < NUM_ROWS; row++){
                    leds[ledIndex(col, row)] = getHSV(hue);
                }
            }
            FastLED.show();
        }
};

class Rabbithole{ // Animation
    public:
        uint8_t step; // How many times has animation grown?
        uint8_t max_steps; // How many steps can animation grow?
        uint8_t main_hue = 0; // Current index
        uint8_t start_xy[2]; // // From which LED the rabbit hole starts
        
        Screen* s; // "s" is short for screen

        Rabbithole(Screen* screen){
            s = screen;

            randomSeed(analogRead(A0)); // Unused analog pin
        }

        void init(){
            s->paintAllSameColor(HUES[main_hue]);
            start();
        }

        void run(){
            if(step <= max_steps){
                /* Every step, the animation "diameter" grows larger. It covers more rows and columns */
                for(int8_t x = start_xy[0] - step; x <= start_xy[0] + step; x++){
                    for(int8_t y = start_xy[1] - step; y <= start_xy[1] + step; y++){
                        if(x >= 0 && x < s->NUM_COLS && y >= 0 && y < s->NUM_ROWS){ // Only update actual display area, because animation technically also grows outside 
                            s->leds[s->ledIndex(x, y)] = getGrowingHue();
                        }
                    }
                }

                FastLED.show();
                step++;
            }else{
                /* New main hue will be last growning hue */
                main_hue++;
                main_hue %= NUM_HUES; // Avoid overflow

                start();
            }
        }

    private:
        void start(){
            step = 1;

            /* Select random LED from which to start */
            start_xy[0] = random() % s->NUM_COLS; // x coordinate
            start_xy[1] = random() % s->NUM_ROWS; // y coordinate

            /* Set start LED color */
            s->leds[s->ledIndex(start_xy[0], start_xy[1])] = getGrowingHue();
            FastLED.show();

            /* How much can animation grow? */
            findMaxSteps();
        }

        void findMaxSteps(){
            uint8_t max_x_steps; // How many steps can animation grow on X axis?
            if(start_xy[0] < s->NUM_COLS / 2){ // If source led is on the left side of display
                max_x_steps = (s->NUM_COLS - 1) - start_xy[0]; // Distance to the right edge
            }else{
                max_x_steps = start_xy[0];
            }

            uint8_t max_y_steps; // How many steps can animation grow on Y axis?
            if(start_xy[1] < s->NUM_ROWS / 2){ // If source led is on the top side of display
                max_y_steps = (s->NUM_ROWS - 1) - start_xy[1]; // Distance to the bottom edge
            }else{
                max_y_steps = start_xy[1];
            }

            max_steps = max_x_steps > max_y_steps ? max_x_steps : max_y_steps;
        }

        CHSV getGrowingHue(){
            return s->getHSV(HUES[(main_hue + 1) % NUM_HUES]);
        }
};