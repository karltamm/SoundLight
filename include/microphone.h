#ifndef MICROPHONE_H
#define MICROPHONE_H

#include <stdint.h>

/* PUBLIC PROTOTYPES */
void init_microphone();
bool detect_volume_bump();

#endif