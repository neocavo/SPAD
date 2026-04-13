#include "CameraProvider.hpp"
#include <iostream>

CameraProvider::CameraProvider(int cameraIndex) {
    cap.open(cameraIndex);
    if (!cap.isOpened()) {
        std::cerr << "Помилка: не вдалось відкрити камеру!" << std::endl;
    }
}

CameraProvider::~CameraProvider() {
    cap.release();
}

cv::Mat CameraProvider::getFrame() {
    cv::Mat frame;
    cap >> frame;
    if (!frame.empty()) {
        cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
        cv::cvtColor(frame, frame, cv::COLOR_RGB2BGR);
    }
    return frame;
}

bool CameraProvider::isOpened() const {
    return cap.isOpened();
}