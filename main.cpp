#include <SDL2/SDL.h>
#include <iostream>
#include <chrono>
#include <thread>
#include "chip_8.h"
#include "audio.h"

constexpr int SCALE = 10;
constexpr int WIN_W = 64 * SCALE;
constexpr int WIN_H = 32 * SCALE;

const std::array<SDL_Keycode, 16> keyMap = {{
    SDLK_1, SDLK_2, SDLK_3, SDLK_4,
    SDLK_q, SDLK_w, SDLK_e, SDLK_r,
    SDLK_a, SDLK_s, SDLK_d, SDLK_f,
    SDLK_z, SDLK_x, SDLK_c, SDLK_v
}};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <rom.ch8>" << std::endl;
        return 1;
    }

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Chip-8 Emulator",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WIN_W, WIN_H, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Audio init
    AudioManager audioManager;
    if (!audioManager.init()) {
        std::cerr << "Warning: Audio initialization failed, continuing without sound." << std::endl;
    }

    Chip_8 chip8;
    try {
        chip8.loadROM(argv[1]);
    } catch (const std::exception& e) {
        std::cerr << "ROM loading error: " << e.what() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    std::array<bool, 16> keys = {false};
    bool running = true;
    SDL_Event event;

    using clock = std::chrono::steady_clock;
    auto nextFrame = clock::now();
    const auto frameDuration = std::chrono::milliseconds(16);

    // Sound control (do not repeat while timer is ON)
    bool wasBeeping = false;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) running = false;
            if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
                bool pressed = (event.type == SDL_KEYDOWN);
                for (int i = 0; i < 16; ++i) {
                    if (event.key.keysym.sym == keyMap[i]) {
                        keys[i] = pressed;
                    }
                }
            }
        }
        chip8.setKeyboardState(keys);
        chip8.checkKeyWaiting();

        for (int i = 0; i < 12; ++i) {
            if (!running) break;
            uint16_t opcode = chip8.fetchOpcode();
            chip8.executeOpcode(opcode);
        }

        chip8.updateTimers();
        // Sound management
        if (chip8.getSoundTimer() > 0) {
            if (!wasBeeping) {
                audioManager.playBeep(50);   // 50 ms sound
                wasBeeping = true;
            }
        } else {
            wasBeeping = false;
        }

        // Render
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

        for (int y = 0; y < SCREEN_HEIGHT; ++y) {
            for (int x = 0; x < SCREEN_WIDTH; ++x) {
                if (chip8.getScreenCell(y, x)) {
                    SDL_Rect rect = { x * SCALE, y * SCALE, SCALE, SCALE };
                    SDL_RenderFillRect(renderer, &rect);
                }
            }
        }
        SDL_RenderPresent(renderer);

        std::this_thread::sleep_until(nextFrame);
        nextFrame += frameDuration;
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}