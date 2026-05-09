#include"chip_8.h"
#include"exception.h"
#include<cstring>


void secureValue(size_t value, size_t bound){
    if (value > bound-1){
        std::string msg = "The value " + std::to_string(value) + " was out of memory bounds. Memory size: " + std::to_string(bound);
        throw RangeError(msg);
    }
}

Chip_8::Chip_8(){
    initialize();
}

void Chip_8::initialize(){
    resetMemory();
    resetRegisters();
    resetStack();
    programCounter = initReserved;
    stackPointer = 0;
    clearScreen();
}

void Chip_8::resetMemory(){
    memset(memory, 0, sizeof(memory));
}

void Chip_8::resetRegisters(){
    memset(registers, 0, sizeof(registers));
}

void Chip_8::resetStack(){
    memset(stack, 0, sizeof(stack));
}

void Chip_8::clearScreen(){
    for (bool& val:screen){
        val = false;
    }
}

bool Chip_8::getScreenCell(size_t row, size_t col){
    size_t pos = row * screen_cols + col;
    secureValue(pos, sizeof(row * col)); return screen[pos];
}

uint8_t Chip_8::getMemoryCell(size_t pos){
    secureValue(pos, sizeof(memory)); return memory[pos];
}

uint8_t Chip_8::getRegisterValue(size_t pos){
    secureValue(pos, sizeof(registers)); return registers[pos];
}

uint16_t Chip_8::getStackCell(size_t pos){
    secureValue(pos, sizeof(stack)); return stack[pos];
}

uint16_t Chip_8::getProgramCounter(){
    return programCounter;
}

uint8_t Chip_8::getStackPointer(){
    return stackPointer;
}