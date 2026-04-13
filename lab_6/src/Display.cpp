#include "Display.hpp"

Display::Display(const std::string& windowName) : windowName(windowName) {
    cv::namedWindow(windowName, cv::WINDOW_AUTOSIZE);
}

void Display::show(const cv::Mat& frame) {
    cv::imshow(windowName, frame);
}