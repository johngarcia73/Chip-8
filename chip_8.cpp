#include"chip_8.h"
#include"exception.h"
#include<cstring>
#include<fstream>

// Helper function to build error messages
std::string buildErrorMessage(const std::string& type, size_t value, size_t bound, const std::string& unit = "bytes") {
    std::ostringstream oss;
    oss << type << " value " << value << " is out of bounds. "
        << "Valid range: 0 to " << (bound - 1) << " " << unit;
    return oss.str();
}


// Validations
void Chip_8::validateMemoryAddress(size_t pos) const {
    if (pos >= MEMORY_SIZE) {
        throw MemoryError(buildErrorMessage("Memory address", pos, MEMORY_SIZE, "bytes"));
    }
}
void Chip_8::validateRegisterIndex(size_t pos) const {
    if (pos >= REGISTERS_NUMBER) {
        throw RegisterError(buildErrorMessage("Register index", pos, REGISTERS_NUMBER, "registers"));
    }
}
void Chip_8::validateStackIndex(size_t pos) const {
    if (pos >= STACK_SIZE) {
        throw StackError(buildErrorMessage("Stack index", pos, STACK_SIZE, "cells"));
    }
}
void Chip_8::validateScreenPosition(size_t row, size_t col) const {
    if (row >= SCREEN_HEIGHT) {
        throw ScreenError(buildErrorMessage("Screen row", row, SCREEN_HEIGHT, "rows"));
    }
    if (col >= SCREEN_WIDTH) {
        throw ScreenError(buildErrorMessage("Screen column", col, SCREEN_WIDTH, "columns"));
    }
}
void Chip_8::validateRom(std::ifstream& infile){
    if(!infile.is_open()){
        throw NotFoundError("Rom file could not be opened.");
    }
}


// Operations
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


#include <sstream>
#include <fstream>
#include <cstring>
#include <chrono>
#include <thread>
#include <iostream>

void Chip_8::loadROM(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        throw NotFoundError("ROM file not found: " + filename);
    }
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    
    size_t maxSize = MEMORY_SIZE - INIT_RESERVED;
    if (fileSize > maxSize) {
        throw MemoryError("ROM too large: " + std::to_string(fileSize) + " bytes (max " + std::to_string(maxSize) + ")");
    }
    
    file.read(reinterpret_cast<char*>(&memory[INIT_RESERVED]), fileSize);
    if (!file.good() && !file.eof()) {
        throw std::runtime_error("Error reading ROM file");
    }
    file.close();
}

void Chip_8::loadROM(const uint8_t* data, size_t size) {
    if (size > MEMORY_SIZE - INIT_RESERVED) {
        throw MemoryError("ROM buffer too large");
    }
    std::memcpy(&memory[INIT_RESERVED], data, size);
}

void Chip_8::run(const std::string& romFile) {
    loadROM(romFile);
    isRunning = true;
    
    // Main loop
    using clock = std::chrono::steady_clock;
    auto nextFrame = clock::now();
    const auto frameDuration = std::chrono::milliseconds(16); // ~60 FPS
    
    while (isRunning) {
        // Executes between 8 and 10 opcodes per frame
        for (int i = 0; i < 10; ++i) {
            uint16_t opcode = fetchOpcode();
            executeOpcode(opcode);
        }
        
        updateTimers();
        updateSound();
        drawScreen();
        
        // Real time control
        std::this_thread::sleep_until(nextFrame);
        nextFrame += frameDuration;
    }
}

uint16_t Chip_8::fetchOpcode() {
    // Big-endian: Combines two consecutive bytes
    uint16_t opcode = (memory[programCounter] << 8) | memory[programCounter + 1];
    programCounter += 2;
    return opcode;
}

void Chip_8::executeOpcode(uint16_t opcode) {
    uint8_t nibble = (opcode & 0xF000) >> 12;
    uint8_t X = (opcode & 0x0F00) >> 8;
    uint8_t Y = (opcode & 0x00F0) >> 4;
    uint8_t N = opcode & 0x000F;
    uint8_t KK = opcode & 0x00FF;
    uint16_t NNN = opcode & 0x0FFF;
    
    switch (nibble) {
        case 0x0:
            if (opcode == 0x00E0) clearScreen();
            else if (opcode == 0x00EE) {
                // subroutine return
                if (stackPointer == 0) throw StackError("Stack underflow on RET");
                programCounter = stack[stackPointer--];
            }
            else {
                // 0nnn: call to RCA 1802 - is ignored on modern emulators
            }
            break;
            
        case 0x1: // JP nnn
            programCounter = NNN;
            break;
            
        case 0x2: // CALL nnn
            if (stackPointer >= STACK_SIZE - 1) throw StackError("Stack overflow on CALL");
            stack[++stackPointer] = programCounter;
            programCounter = NNN;
            break;
            
        case 0x3: // SE Vx, kk
            if (registers[X] == KK) programCounter += 2;
            break;
            
        case 0x4: // SNE Vx, kk
            if (registers[X] != KK) programCounter += 2;
            break;
            
        case 0x5: // SE Vx, Vy
            if (registers[X] == registers[Y]) programCounter += 2;
            break;
            
        case 0x6: // LD Vx, kk
            registers[X] = KK;
            break;
            
        case 0x7: // ADD Vx, kk
            registers[X] += KK;
            break;
            
        case 0x8:
            switch (N) {
                case 0x0: registers[X] = registers[Y]; break;
                case 0x1: registers[X] |= registers[Y]; break;
                case 0x2: registers[X] &= registers[Y]; break;
                case 0x3: registers[X] ^= registers[Y]; break;
                case 0x4: // ADD Vx, Vy (with carry)
                    {
                        uint16_t sum = registers[X] + registers[Y];
                        registers[0xF] = (sum > 0xFF) ? 1 : 0;
                        registers[X] = sum & 0xFF;
                    }
                    break;
                case 0x5: // SUB Vx, Vy (Vx - Vy)
                    registers[0xF] = (registers[X] > registers[Y]) ? 1 : 0;
                    registers[X] -= registers[Y];
                    break;
                case 0x6: // SHR Vx (right shift)
                    registers[0xF] = registers[X] & 0x1;
                    registers[X] >>= 1;
                    break;
                case 0x7: // SUBN Vx, Vy (Vy - Vx)
                    registers[0xF] = (registers[Y] > registers[X]) ? 1 : 0;
                    registers[X] = registers[Y] - registers[X];
                    break;
                case 0xE: // SHL Vx (left shift)
                    registers[0xF] = (registers[X] & 0x80) >> 7;
                    registers[X] <<= 1;
                    break;
                default: break;
            }
            break;
            
        case 0x9: // SNE Vx, Vy
            if (registers[X] != registers[Y]) programCounter += 2;
            break;
            
        case 0xA: // LD I, nnn
            indexRegister = NNN;
            break;
            
        case 0xB: // JP V0, nnn
            programCounter = NNN + registers[0];
            break;
            
        case 0xC: // RND Vx, kk
            registers[X] = (rand() % 256) & KK;
            break;
            
        case 0xD: // DRAW
            drawSprite(X, Y, N);
            break;
            
        case 0xE:
            if (KK == 0x9E) { // SKP Vx
                if (isKeyPressed(registers[X])) programCounter += 2;
            } else if (KK == 0xA1) { // SKNP Vx
                if (!isKeyPressed(registers[X])) programCounter += 2;
            }
            break;
            
        case 0xF:
            switch (KK) {
                case 0x07: registers[X] = delayTimer; break;
                case 0x0A: waitForKey(registers[X]); break;
                case 0x15: delayTimer = registers[X]; break;
                case 0x18: soundTimer = registers[X]; break;
                case 0x1E: indexRegister += registers[X]; break;
                case 0x29: indexRegister = registers[X] * 5; // Sprite digits (0x000-0x09F)
                    break;
                case 0x33: storeBCD(registers[X]); break;
                case 0x55: // Store V0..Vx from memory on I
                    for (int i = 0; i <= X; ++i) memory[indexRegister + i] = registers[i];
                    break;
                case 0x65: // Load V0..Vx from memory on I
                    for (int i = 0; i <= X; ++i) registers[i] = memory[indexRegister + i];
                    break;
                default: break;
            }
            break;
            
        default:
            throw std::runtime_error("Unknown opcode nibble");
    }
}

void Chip_8::drawSprite(uint8_t Vx, uint8_t Vy, uint8_t N) {
    uint8_t x = registers[Vx] % SCREEN_WIDTH;
    uint8_t y = registers[Vy] % SCREEN_HEIGHT;
    registers[0xF] = 0;  // reset collision flag
    
    for (int row = 0; row < N; ++row) {
        uint8_t spriteByte = memory[indexRegister + row];
        int yPos = (y + row) % SCREEN_HEIGHT;
        
        for (int col = 0; col < 8; ++col) {
            if ((spriteByte & (0x80 >> col)) == 0) continue;
            int xPos = (x + col) % SCREEN_WIDTH;
            size_t pos = yPos * SCREEN_WIDTH + xPos;
            
            // XOR drawing
            if (screen[pos]) registers[0xF] = 1;  // collision
            screen[pos] = screen[pos] ^ true;
        }
    }
}

void Chip_8::updateTimers() {
    if (delayTimer > 0) delayTimer--;
    if (soundTimer > 0) soundTimer--;
}

void Chip_8::updateSound() {
    if (soundTimer > 0) {
        // beep (depends on platform)
        // For now just print a message or use a simple system beep
        std::cout << '\a' << std::flush;
    }
}