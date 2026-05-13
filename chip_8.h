#pragma once
#include<stdio.h>
#include<cstdint>
#include<array>
#include<iomanip>
#include<fstream>
#include<SDL2/SDL.h> 
using namespace std;

constexpr uint16_t INIT_RESERVED = 0x200;    // The minimum address available (Most of the time, it is 0x200,
                                                // due to the original interpreter code)
constexpr size_t MEMORY_SIZE = 4096;
constexpr size_t REGISTERS_NUMBER = 16;
constexpr size_t STACK_SIZE = 16;
constexpr size_t SCREEN_WIDTH = 64;
constexpr size_t SCREEN_HEIGHT = 32;
constexpr size_t SCREEN_SIZE = SCREEN_WIDTH * SCREEN_HEIGHT;

struct Chip_8{
    
    private:

        // Core
        uint8_t memory[MEMORY_SIZE];      
        uint8_t registers[REGISTERS_NUMBER];               
        uint16_t stack[STACK_SIZE];
        uint16_t programCounter;
        uint8_t stackPointer;
        std::array<bool, SCREEN_SIZE> screen;
        bool isRunning;

        // Keyboard
        bool waitingForKey = false;
        uint8_t* keyRegister = nullptr;

        // Audio
        SDL_AudioDeviceID audioDevice;
        bool audioInitialized = false;

        void validateMemoryAddress(size_t pos) const;
        void validateRegisterIndex(size_t pos) const;
        void validateStackIndex(size_t pos) const;
        void validateScreenPosition(size_t row, size_t col) const;
        void validateRom(std::ifstream& file);

        // Operations
        void fetch();
        void decode();
        void execute();

    public:
        //Constructor
        Chip_8();
        void initialize();

        // Execution control
        void run(const std::string& romFile);
        void stop() { isRunning = false; }
        
        // Rom loading
        void loadROM(const std::string& filename);
        void loadROM(const uint8_t* data, size_t size);

        // Cycle components
        uint16_t fetchOpcode();
        void executeOpcode(uint16_t opcode);

        // Cleaners
        void resetMemory();
        void resetRegisters();
        void resetStack();
        void clearScreen();

        // Getters
        bool getScreenCell(size_t row, size_t col);
        uint8_t getMemoryCell(size_t pos);
        uint8_t getRegisterValue(size_t pos);
        uint16_t getStackCell(size_t pos);
        uint16_t getProgramCounter() const;
        uint8_t getStackPointer() const;
        bool getIsRunning() const;
        uint8_t getSoundTimer() const;


        // Setters
        void setMemoryCell(size_t pos, uint8_t value);
        void setRegisterValue(size_t pos, uint8_t value);
        void setStackCell(size_t pos, uint16_t value);
        void setScreenCell(size_t row, size_t col, bool value);

        void drawSprite(uint8_t Vx, uint8_t Vy, uint8_t N);
        void drawScreen();
        void updateTimers();
        void storeBCD(uint8_t value);

        uint16_t indexRegister;
        uint8_t delayTimer;
        uint8_t soundTimer;          
        uint16_t currentOpcode; 

        void waitForKey(uint8_t& reg);
        void checkKeyWaiting(); 

        void setKeyboardState(const std::array<bool, 16>& keys) { 
            keyboardState = keys; 
        }
    
        bool isKeyPressed(uint8_t key) const { 
            if (key >= 16) return false;
            return keyboardState[key]; 
        }

        std::array<bool, 16> keyboardState{}; 
};