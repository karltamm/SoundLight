#include <devices.h>

/* CONSTANTS */
/* Hue */
HSVHue HUES[] = {HUE_PURPLE, HUE_YELLOW, HUE_BLUE, HUE_RED, HUE_GREEN, HUE_PINK};
uint8_t NUM_HUES = 6;

/* Gradients */
CRGB PINK_PLASMA = CHSV( HUE_PINK, 255, 255);
CRGB BLUE_PLASMA  = CHSV( HUE_BLUE, 255, 255);

CRGBPalette16 TRIPPY_PLASMA = CRGBPalette16( 
    PINK_PLASMA,  PINK_PLASMA,  PINK_PLASMA,  PINK_PLASMA,
    BLUE_PLASMA, BLUE_PLASMA, BLUE_PLASMA,  BLUE_PLASMA,
    PINK_PLASMA,  PINK_PLASMA,  PINK_PLASMA,  PINK_PLASMA,
    BLUE_PLASMA, BLUE_PLASMA, BLUE_PLASMA,  BLUE_PLASMA
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

class Triangles{
    public:
        Microphone* mic;
        Screen* s; // "s" is short for screen
        uint8_t screen_width;
        uint8_t height;
        uint8_t max_height;
        uint8_t side;

        uint8_t frame_ms = 60;

        Triangles(Screen* screen, Microphone* mic){
            s = screen;
            Triangles::mic = mic;
            Triangles::max_height = s->NUM_COLS / 2; // Max height = half of screen diagonal
        }

        void run(){
            FastLED.clear(); // Clear previous animation

            /* Calculate paramenets. Triangles are right angled with equal sides */
            height = map8(mic->getVol(), 0, max_height);
            height = height < 2 ? 2 : height; // At least minimum value
            side = 2 * height / sqrt(2); // Found using cosine

            /* Draw triangles. Triangles share same hypotenuse and form a square (in the middle of screen) */
            /* Find starting point (x and y are same) for 1st triangle */
            uint8_t triangle_1_start = ((s->NUM_COLS - 1) - side) / 2; // 2 triangles share same hypotenuse and form a square; place this "square" in the middle of screen.
            uint8_t triangle_1_end = triangle_1_start + side;

            /* Draw 1st triangle */
            uint8_t step = 0; // Every step moves y starting point lower; this draws a line that is hypotenuse
            for(uint8_t x = triangle_1_start; x <= triangle_1_end; x++){
                for(uint8_t y = triangle_1_start + step; y <= triangle_1_end; y++){
                    s->setLed(x, y, HUE_BLUE);
                }
                step++;
            }

            /* Draw 2nd triangle */
            step = 0;
            uint8_t triangle_2_start_x = triangle_1_end; // Drawing goes from right to left (opposite to 1st triangle)
            uint8_t triangle_2_end_x = triangle_1_start;

            uint8_t triangle_2_start_y = triangle_1_start;
            uint8_t triangle_2_end_y = triangle_1_end;

            for(uint8_t x = triangle_2_start_x; x >= triangle_2_end_x; x--){
                for(uint8_t y = triangle_2_start_y; y <= triangle_2_end_y - step; y++){
                    s->setLed(x, y, HUE_RED);
                }

                step++;
            }

            FastLED.show();
            delay(frame_ms);
        }
};

class Plasma{
    public:
        uint8_t scale = 25; // Affects how big a plasma blob will be
        uint16_t depth = 0; // For the noise algorithm (z coordinate)
        int8_t depth_step = 10; // How much "deeper" animation goes every cycle
        uint8_t brightness = 5;
        uint8_t frame_ms = 50;

        Screen* s;

        Plasma(Screen* screen){
            s = screen;
        }

        void run(){
            /* Give values for LEDs */
            uint8_t noise;
            uint8_t hue_index;

            for(uint16_t x = 0; x < s->NUM_COLS; x++){
                for(uint16_t y = 0; y < s->NUM_ROWS; y++){
                    /* Generate hue index for a LED based on Simplex noise algorith */
                    noise = inoise8(x * scale, y * scale, depth);
                    hue_index = map(noise, SIMPLEX_MIN, SIMPLEX_MAX, 0, 255);

                    s->setLed(x, y, ColorFromPalette(TRIPPY_PLASMA, hue_index, brightness));
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
            delay(frame_ms);
        }
};