#pragma once

enum class Mode {
    NORMAL,
    INVERT,
    BLUR,
    CANNY,
    SOBEL,
    GRAY,
    FACE  // новий режим детекції облич
};

class KeyProcessor {
public:
    KeyProcessor();
    Mode getMode() const;
    bool processKey(int key);

private:
    Mode currentMode;
};