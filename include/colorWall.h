#include "gradient.h"

/* CONSTANTS */
#define WALL_ROWS GRAD_WIDTH
#define WALL_COLS GRAD_HEIGHT

/* CLASSES */
class ColorWall{
    public:
        RGB matrix[WALL_ROWS][WALL_COLS];

        ColorWall(Gradient gradient){
            /* Make "wall" from gradient */
            for(int col = 0; col < WALL_COLS; col++){
                for(int row = 0; row < WALL_ROWS; row++){
                    matrix[row][col].R = gradient.block[row][col].R;
                    matrix[row][col].G = gradient.block[row][col].G;
                    matrix[row][col].B = gradient.block[row][col].B;
                }
            }
        }

        void flowToRight(){
            /* Move columns from left to right. Start moving from right side. Right-most column flows out*/
            for(int col = WALL_COLS - 2; col > - 1; col--){ // "WALL_COLS - 2" because last column values flow out
                for(int row = 0; row < WALL_ROWS; row++){
                    matrix[row][col+1].R = matrix[row][col].R;
                    matrix[row][col+1].G = matrix[row][col].G;
                    matrix[row][col+1].B = matrix[row][col].B;
                }
            }
        }

        void print(){
            char string[100];

            for(int row = 0; row < WALL_ROWS; row++){
                for(int col = 0; col < WALL_COLS; col++){
                    sprintf(string, "(%3d, %3d, %3d) ", matrix[row][col].R, matrix[row][col].G, matrix[row][col].B);
                    Serial.print(string);
                }
                Serial.println();
            }

            Serial.println("\n");
        }

        void flowInto(ColorWall* dest_wall){
            for(int row = 0; row < WALL_ROWS; row++){
                dest_wall->matrix[row][0].R = matrix[row][WALL_COLS - 1].R;
                dest_wall->matrix[row][0].G = matrix[row][WALL_COLS - 1].G;
                dest_wall->matrix[row][0].B = matrix[row][WALL_COLS - 1].B;
            }
        }
};
