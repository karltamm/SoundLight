# SoundLight

SoundLight is a sound reactive artwork. It displays light patterns on a LED display based on surrounding sounds. Patterns are generated based on the Simplex noise algorithm. Patterns change color when the device hears a sound that is louder than usual.

- [Demo video](https://www.youtube.com/watch?v=6hr5MKKdF0U)
- [Photos](https://imgur.com/a/T8ksyk4)

I built this project originally at the end of my first year as a computer engineering student. Now as I'm graduating, I decided to refactor the original source code. The source files are written in C++, cause the project uses Arduino framework and C++ library called [FastLED](https://fastled.io/). However, I decided to rewrite the project in procedural style (C-style) as this suits better for the project.

Device is powered by [Arduino Nano](https://store.arduino.cc/products/arduino-nano) and it uses microphone amplifier [MAX9814](https://www.analog.com/media/en/technical-documentation/data-sheets/max9814.pdf) and 16x16 LED matrix display (uses WS2812B LEDs). The enclosure is partly 3D printed and the rest is made of plywood.
