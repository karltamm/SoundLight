#include <Arduino.h>

/* CONSTANTS */
#define GRAD_WIDTH  6 // Gradient has horizontally 6 different shades
#define GRAD_HEIGHT 6

/* CLASSES */
class RGB{
    public:
        int R;
        int G;
        int B;

        RGB(int red = 0, int green = 0, int blue = 0){
            R = red;
            G = green;
            B = blue;
        }
};

class Gradient{
    public:
        RGB block[GRAD_HEIGHT][GRAD_WIDTH];
        char* name;

        Gradient(RGB l_side, RGB r_side){
            float red_step = 1.0 * (l_side.R - r_side.R) / GRAD_WIDTH;
            float green_step = 1.0 * (l_side.G - r_side.G) / GRAD_WIDTH;
            float blue_step = 1.0 * (l_side.B - r_side.B) / GRAD_WIDTH;

            RGB color;

            for(int col = 0; col < GRAD_WIDTH; col++){
                /* Calculate gradient "column" RGB value */
                if(col == 0){ // First column
                    color.R = l_side.R;
                    color.G = l_side.G;
                    color.B = l_side.B;
                }else if(col == GRAD_WIDTH - 1){ // Last column
                    color.R = r_side.R;
                    color.G = r_side.G;
                    color.B = r_side.B;
                }else{ // In between
                    color.R = l_side.R - red_step * col;
                    color.G = l_side.G - green_step * col;
                    color.B = l_side.B - blue_step * col;
                }

                /* Add color to gradient column */
                for(int row = 0; row < GRAD_HEIGHT; row++){
                    block[row][col] = color; 
                }
            }
        }

        void print(){
            char string[100];

            for(int row = 0; row < GRAD_HEIGHT; row++){
                for(int col = 0; col < GRAD_WIDTH; col++){
                    sprintf(string, "(%3d, %3d, %3d) ", block[row][col].R, block[row][col].G, block[row][col].B);
                    Serial.print(string);
                }
                Serial.println();
            }

            Serial.println("\n");
        }
};