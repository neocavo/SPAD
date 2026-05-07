#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <atomic>

class FaceDetector {
public:
    FaceDetector(const std::string& prototxt, const std::string& caffemodel);
    ~FaceDetector();

    // Передати новий кадр на детекцію
    void submitFrame(const cv::Mat& frame);

    // Отримати останні результати детекції
    std::vector<cv::Rect> getDetections();

    bool isLoaded() const;

private:
    void detectionLoop();

    cv::dnn::Net net;
    bool loaded;

    std::thread workerThread;
    std::mutex mutex;
    std::condition_variable cv;

    cv::Mat pendingFrame;
    bool hasNewFrame;
    std::atomic<bool> running;

    std::vector<cv::Rect> lastDetections;
};