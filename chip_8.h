#include<stdio.h>
#include<cstdint>
#include<array>
#include<iomanip>
#include<fstream>
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
        bool isRunning;

        

        void validateMemoryAddress(size_t pos) const;
        void validateRegisterIndex(size_t pos) const;
        void validateStackIndex(size_t pos) const;
        void validateScreenPosition(size_t row, size_t col) const;
        void Chip_8::validateRom(std::ifstream& file);

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


        // Setters
        void setMemoryCell(size_t pos, uint8_t value);
        void setRegisterValue(size_t pos, uint8_t value);
        void setStackCell(size_t pos, uint16_t value);
        void setScreenCell(size_t row, size_t col, bool value);

        void run(string url);
        void mainLoop(ifstream& infile);

        void drawSprite(uint8_t Vx, uint8_t Vy, uint8_t N);
        void Chip_8::updateTimers();
        void Chip_8::updateSound(); 

        uint16_t indexRegister;
        uint8_t delayTimer;
        uint8_t soundTimer;          
        uint16_t currentOpcode; 

        bool isKeyPressed(uint8_t key) const;
        void waitForKey(uint8_t& reg);
};