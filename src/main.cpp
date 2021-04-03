#include <Arduino.h>
#include <colorWall.h>
#include <TM1637Display.h>

/* CONSTANTS */
/* Pins */
#define MIC_PIN     A2
#define DISPLAY_PIN  3
#define CLK_PIN      5

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

/* GLOBAL VARIABLES */
/* Components */
TM1637Display display(CLK_PIN, DISPLAY_PIN);

/* Misc */
int counter = 0;

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

    /* Pins */
    pinMode(MIC_PIN, INPUT);

    /* Display */
    display.clear();
    display.setBrightness(7);
}

void loop() {
    display.showNumberDec(counter);
    counter = counter < 100 ? counter + 1 : 0;
}