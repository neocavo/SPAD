#pragma once
#include <opencv2/opencv.hpp>
#include <string>

class Display {
public:
    Display(const std::string& windowName);
    void show(cv::Mat& frame);

    static void mouseCallback(int event, int x, int y, int flags, void* userdata);
    static cv::Point lastClick;
    static bool clicked;

private:
    std::string windowName;
};