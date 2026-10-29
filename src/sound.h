#ifndef SOUND_H
#define SOUND_H

#include <Arduino.h>

// Khai báo hàm dùng bên ngoài
void initSoundTask();
void playSoundAsync(const String& str);
bool isSoundPlaying();
int getSoundDurationMs(const String& melody);
void playWav(const char* path);
#endif
