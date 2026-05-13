#pragma once
#include <SDL2/SDL.h>

class AudioManager {
public:
    AudioManager();
    ~AudioManager();
    bool init();
    void playBeep(int durationMs = 50);
    void shutdown();
private:
    SDL_AudioDeviceID device;
    bool initialized;
};