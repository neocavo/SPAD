#include "FrameProcessor.hpp"

cv::Mat FrameProcessor::process(const cv::Mat& frame, Mode mode) {
    cv::Mat result;
    switch (mode) {
        case Mode::NORMAL:
            result = frame.clone();
            break;
        case Mode::INVERT:
            // Інверсія кольорів
            cv::bitwise_not(frame, result);
            break;
        case Mode::BLUR:
            // Gaussian blur
            cv::GaussianBlur(frame, result, cv::Size(15, 15), 0);
            break;
        case Mode::CANNY: {
            // Canny фільтр — потрібен grayscale
            cv::Mat gray;
            cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
            cv::Canny(gray, result, 100, 200);
            cv::cvtColor(result, result, cv::COLOR_GRAY2BGR);
            break;
        }
        case Mode::SOBEL: {
            // Фільтр Собеля
            cv::Mat gray, sobelX, sobelY;
            cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
            cv::Sobel(gray, sobelX, CV_16S, 1, 0);
            cv::Sobel(gray, sobelY, CV_16S, 0, 1);
            cv::convertScaleAbs(sobelX, sobelX);
            cv::convertScaleAbs(sobelY, sobelY);
            cv::addWeighted(sobelX, 0.5, sobelY, 0.5, 0, result);
            cv::cvtColor(result, result, cv::COLOR_GRAY2BGR);
            break;
        }
        case Mode::GRAY:
            // Відтінки сірого
            cv::cvtColor(frame, result, cv::COLOR_BGR2GRAY);
            cv::cvtColor(result, result, cv::COLOR_GRAY2BGR);
            break;
    }
    return result;
}