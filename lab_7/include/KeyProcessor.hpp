#pragma once

enum class Mode {
    NORMAL,
    INVERT,
    BLUR,
    CANNY,
    SOBEL,
    GRAY
};

class KeyProcessor {
public:
    KeyProcessor();
    Mode getMode() const;
    bool processKey(int key);  // повертає false якщо треба вийти

private:
    Mode currentMode;
};