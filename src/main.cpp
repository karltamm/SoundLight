#include <Arduino.h>
#include <colorWall.h>

/* CONSTANTS */
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

void setup() {
    /* Serial monitor */
    Serial.begin(9600);

    /* Create ColorWalls */
    ColorWall visible(BLUE_GRAD);
    ColorWall buffer(YELLOW_GRAD);

    /* ColorWalls flow to right */
    visible.flowToRight();
    buffer.flowInto(&visible);
    buffer.flowToRight();
}

void loop() {

}