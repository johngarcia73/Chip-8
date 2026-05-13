#include "chip_8.h"
#include "exception.h"


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
