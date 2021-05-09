#include <devices.h>

/* CONSTANTS */
/* Hue */
HSVHue HUES[] = {HUE_PURPLE, HUE_YELLOW, HUE_BLUE, HUE_RED, HUE_GREEN, HUE_PINK};
uint8_t NUM_HUES = 6;

/* Gradients */
CRGB PINK = CHSV(HUE_PINK, 255, 255);
CRGB BLUE  = CHSV(HUE_BLUE, 255, 255);
CRGB YELLOW = CHSV(HUE_YELLOW, 255, 255);

CRGBPalette16 PINK_BLUE = CRGBPalette16( 
    PINK,  PINK,  PINK,  PINK,
    BLUE, BLUE, BLUE,  BLUE,
    PINK,  PINK,  PINK,  PINK,
    BLUE, BLUE, BLUE,  BLUE
);

CRGBPalette16 PINK_YELLOW = CRGBPalette16( 
    PINK,  PINK,  PINK,  PINK,
    YELLOW, YELLOW, YELLOW,  YELLOW,
    PINK,  PINK,  PINK,  PINK,
    YELLOW, YELLOW, YELLOW,  YELLOW
);

/* Plasma animation */
const uint8_t SIMPLEX_MIN = 50; // Noise value is usually above 50
const uint8_t SIMPLEX_MAX = 190;

/* CLASSES */
class Rabbithole{ // Animation
    public:
        uint8_t step; // How many times has animation grown?
        uint8_t max_steps; // How many steps can animation grow?
        uint8_t main_hue = 0; // Current index
        uint8_t start_xy[2]; // // From which LED the rabbit hole starts
        
        Screen* s; // "s" is short for screen
        Microphone* mic;

        uint8_t frame_ms = 66;

        Rabbithole(Screen* screen, Microphone* microphone){
            s = screen;
            mic = microphone;
            randomSeed(analogRead(A0)); // Unused analog pin for random LED selection
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
                            s->setLed(x, y, getGrowingHue());
                        }
                    }
                }

                FastLED.show();
                step++;
                delay(frame_ms);
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
            s->setLed(start_xy[0], start_xy[1], getGrowingHue());
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

class Plasma{
    public:
        uint8_t scale = 25; // Affects how big a plasma blob will be
        uint16_t depth = 0; // For the noise algorithm (z coordinate)
        int8_t depth_step = 10; // How much "deeper" animation goes every cycle
        uint8_t brightness;

        Screen* s;
        Microphone* mic;

        Plasma(Screen* screen, Microphone* mic){
            s = screen;
            brightness = screen->brightness;
            Plasma::mic = mic;
        }

        void run(CRGBPalette16 palette){
            /* Give values for LEDs */
            uint8_t noise;
            uint8_t hue_index;

            for(uint16_t x = 0; x < s->NUM_COLS; x++){
                for(uint16_t y = 0; y < s->NUM_ROWS; y++){
                    /* Generate hue index for a LED based on Simplex noise algorith */
                    noise = inoise8(x * scale, y * scale, depth);
                    hue_index = map(noise, SIMPLEX_MIN, SIMPLEX_MAX, 0, 255);

                    s->setLed(x, y, ColorFromPalette(palette, hue_index, brightness));
                }
            }

            /* Animations moves trough time */
            depth += depth_step;

            if(depth > 65000) // Around 2^16
                depth_step = -10; // Backwards
            else if(depth < 50)
                depth_step = 10; // Forward

            /* Update screen */
            FastLED.show();
        }

        void soundReactivePlasma(){
            if(mic->isVolumeBump()){
                run(PINK_YELLOW);
            }else{
                run(PINK_BLUE);
            }
        }
};