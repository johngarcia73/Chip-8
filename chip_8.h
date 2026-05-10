#include<stdio.h>
#include<cstdint>
#include<array>
#include<iomanip>
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
        uint8_t memory[MEMORY_SIZE];      
        uint8_t registers[REGISTERS_NUMBER];               
        uint16_t stack[STACK_SIZE];

        uint16_t programCounter;
        uint8_t stackPointer;
        std::array<bool, SCREEN_SIZE> screen;

        void validateMemoryAddress(size_t pos) const;
        void validateRegisterIndex(size_t pos) const;
        void validateStackIndex(size_t pos) const;
        void validateScreenPosition(size_t row, size_t col) const;

    public:
        //Constructor
        Chip_8();
        void initialize();

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

        // Setters
        void setMemoryCell(size_t pos, uint8_t value);
        void setRegisterValue(size_t pos, uint8_t value);
        void setStackCell(size_t pos, uint16_t value);
        void setScreenCell(size_t row, size_t col, bool value);
};