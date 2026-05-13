#include "audio.h"
#include <cmath>
#include <vector>
#include <iostream>

AudioManager::AudioManager() : device(0), initialized(false) {}

AudioManager::~AudioManager() {
    shutdown();
}

bool AudioManager::init() {
    if (initialized) return true;
    SDL_AudioSpec want, have;
    SDL_zero(want);
    want.freq = 44100;
    want.format = AUDIO_S16;
    want.channels = 1;
    want.samples = 2048;
    want.callback = nullptr;
    device = SDL_OpenAudioDevice(nullptr, 0, &want, &have, 0);
    if (device == 0) {
        std::cerr << "Failed to open audio device: " << SDL_GetError() << std::endl;
        return false;
    }
    initialized = true;
    return true;
}

void AudioManager::playBeep(int durationMs) {
    if (!initialized) return;
    const int sampleRate = 44100;
    const int samples = (sampleRate * durationMs) / 1000;
    const int16_t amplitude = 20000;
    const double freq = 440.0;
    std::vector<int16_t> buffer(samples);
    for (int i = 0; i < samples; ++i) {
        double t = (double)i / sampleRate;
        buffer[i] = static_cast<int16_t>(amplitude * std::sin(2.0 * M_PI * freq * t));
    }
    SDL_ClearQueuedAudio(device);
    SDL_QueueAudio(device, buffer.data(), samples * sizeof(int16_t));
    SDL_PauseAudioDevice(device, 0);
}

void AudioManager::shutdown() {
    if (device != 0) {
        SDL_CloseAudioDevice(device);
        device = 0;
    }
    initialized = false;
}