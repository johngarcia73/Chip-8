#include "chip_8.h"

void Chip_8::waitForKey(uint8_t& reg) {
    waitingForKey = true;
    keyRegister = &reg;
}

void Chip_8::checkKeyWaiting() {
    if (!waitingForKey) return;
    for (int i = 0; i < 16; ++i) {
        if (keyboardState[i]) {
            *keyRegister = i;
            waitingForKey = false;
            keyRegister = nullptr;
            break;
        }
    }
}