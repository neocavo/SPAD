#include "KeyProcessor.hpp"

KeyProcessor::KeyProcessor() : currentMode(Mode::NORMAL) {}

Mode KeyProcessor::getMode() const {
    return currentMode;
}

bool KeyProcessor::processKey(int key) {
    switch (key) {
        case 27:  // ESC — вихід
            return false;
        case 'n': case 'N':
            currentMode = Mode::NORMAL;
            break;
        case 'i': case 'I':
            currentMode = Mode::INVERT;
            break;
        case 'b': case 'B':
            currentMode = Mode::BLUR;
            break;
        case 'c': case 'C':
            currentMode = Mode::CANNY;
            break;
        case 's': case 'S':
            currentMode = Mode::SOBEL;
            break;
        case 'g': case 'G':
            currentMode = Mode::GRAY;
            break;
    }
    return true;
}