#include "chip_8.h"
#include "exception.h"
#include <iostream>
#include <cstring>

// Loading and running
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
                case 0x0A:
                    waitForKey(registers[X]);
                    programCounter -=2;
                    break;
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

void Chip_8::storeBCD(uint8_t value) {
    uint8_t* bcdPtr = &memory[indexRegister];
    
    bcdPtr[0] = value / 100;
    bcdPtr[1] = (value / 10) % 10;
    bcdPtr[2] = value % 10;
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

void Chip_8::drawScreen() {
    static int frame = 0;
    if (++frame % 10 == 0) { // reduces speed
        system("clear"); // Linux/Mac
        for (int row = 0; row < SCREEN_HEIGHT; ++row) {
            for (int col = 0; col < SCREEN_WIDTH; ++col) {
                size_t pos = row * SCREEN_WIDTH + col;
                std::cout << (screen[pos] ? '#' : ' ');
            }
            std::cout << '\n';
        }
        std::cout << std::flush;
    }
}

void Chip_8::updateTimers() {
    if (delayTimer > 0) delayTimer--;
    if (soundTimer > 0) soundTimer--;
}
