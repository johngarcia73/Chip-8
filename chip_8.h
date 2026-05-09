#include<stdio.h>
#include<cstdint>
#include<array>
#include<iomanip>
using namespace std;

struct Chip_8{
    
    private:
        uint8_t memory[4096];
        uint16_t initReserved=0x200;      // The minimum address available (Most of the time, it is 0x200,
        uint8_t registers[16];               // due to the original interpreter code)
        uint16_t stack[16];

        uint16_t programCounter;
        uint8_t stackPointer;

        size_t screen_rows = 32;
        size_t screen_cols = 64;

        bool screen[64 * 32];  

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
        uint16_t getProgramCounter();
        uint8_t getStackPointer();
};