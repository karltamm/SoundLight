#include <microphone.h>
#include <plasma_animation.h>
#include <screen.h>

void setup() {
  Serial.begin(115200);
  init_screen(200);
  init_microphone();
}

void loop() {
  update_plasma_animation();
}