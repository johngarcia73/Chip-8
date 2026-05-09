#include<stdexcept>

class RangeError: public std::runtime_error{
    public:
        explicit RangeError(const std::string& message)
        : std::runtime_error(message) { }
};