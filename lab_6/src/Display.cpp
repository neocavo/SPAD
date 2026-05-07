#include "Display.hpp"

cv::Point Display::lastClick = cv::Point(0, 0);
bool Display::clicked = false;

Display::Display(const std::string& windowName) : windowName(windowName) {
    cv::namedWindow(windowName, cv::WINDOW_AUTOSIZE);
    // Реєструємо обробник миші
    cv::setMouseCallback(windowName, mouseCallback, nullptr);
}

void Display::show(cv::Mat& frame) {
    // Лівий клік — малюємо червоний кружок на місці кліку
    if (clicked) {
        cv::circle(frame, lastClick, 20, cv::Scalar(0, 0, 255), 3);
    }
    cv::imshow(windowName, frame);
}

void Display::mouseCallback(int event, int x, int y, int flags, void* userdata) {
    if (event == cv::EVENT_LBUTTONDOWN) {
        // Лівий клік — запам'ятовуємо точку
        lastClick = cv::Point(x, y);
        clicked = true;
    } else if (event == cv::EVENT_RBUTTONDOWN) {
        // Правий клік — скидаємо кружок
        clicked = false;
    }
}