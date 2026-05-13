#include "chip_8.h"
#include "exception.h"

// Getters
uint8_t Chip_8::getMemoryCell(size_t pos) {
    validateMemoryAddress(pos);
    return memory[pos];
}
uint8_t Chip_8::getRegisterValue(size_t pos) {
    validateRegisterIndex(pos);
    return registers[pos];
}
uint16_t Chip_8::getStackCell(size_t pos) {
    validateStackIndex(pos);
    return stack[pos];
}
bool Chip_8::getScreenCell(size_t row, size_t col) {
    validateScreenPosition(row, col);
    size_t pos = row * SCREEN_WIDTH + col;
    return screen[pos];
}
uint16_t Chip_8::getProgramCounter() const {
    return programCounter;
}
uint8_t Chip_8::getStackPointer() const {
    return stackPointer;
}
bool Chip_8::getIsRunning() const{
    return isRunning;
}
uint8_t Chip_8::getSoundTimer() const{ return soundTimer; }

// Setters
void Chip_8::setMemoryCell(size_t pos, uint8_t value) {
    validateMemoryAddress(pos);
    memory[pos] = value;
}
void Chip_8::setRegisterValue(size_t pos, uint8_t value) {
    validateRegisterIndex(pos);
    registers[pos] = value;
}
void Chip_8::setStackCell(size_t pos, uint16_t value) {
    validateStackIndex(pos);
    stack[pos] = value;
}
void Chip_8::setScreenCell(size_t row, size_t col, bool value) {
    validateScreenPosition(row, col);
    size_t pos = row * SCREEN_WIDTH + col;
    screen[pos] = value;
}


// Init operations
Chip_8::Chip_8(){
    initialize();
}
void Chip_8::initialize(){
    resetMemory();
    resetRegisters();
    resetStack();
    programCounter = INIT_RESERVED;
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