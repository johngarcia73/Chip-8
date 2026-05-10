#pragma once

#include <stdexcept>
#include <string>

class RangeError : public std::runtime_error {
public:
    explicit RangeError(const std::string& message)
        : std::runtime_error(message) { }
    
    explicit RangeError(const char* message)
        : std::runtime_error(message) { }
};

class MemoryError : public RangeError {
public:
    explicit MemoryError(const std::string& message)
        : RangeError("Memory Error: " + message) { }
};

class StackError : public RangeError {
public:
    explicit StackError(const std::string& message)
        : RangeError("Stack Error: " + message) { }
};

class RegisterError : public RangeError {
public:
    explicit RegisterError(const std::string& message)
        : RangeError("Register Error: " + message) { }
};

class ScreenError : public RangeError {
public:
    explicit ScreenError(const std::string& message)
        : RangeError("Screen Error: " + message) { }
};
