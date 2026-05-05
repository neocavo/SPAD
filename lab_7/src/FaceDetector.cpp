#include "FaceDetector.hpp"

FaceDetector::FaceDetector(const std::string& prototxt, const std::string& caffemodel) 
    : loaded(false), hasNewFrame(false), running(true) {
    
    // Завантажуємо нейронну мережу ResNet-10
    try {
        net = cv::dnn::readNetFromCaffe(prototxt, caffemodel);
        loaded = true;
    } catch (const cv::Exception& e) {
        loaded = false;
        return;
    }

    // Запускаємо окремий потік для детекції
    workerThread = std::thread(&FaceDetector::detectionLoop, this);
}

FaceDetector::~FaceDetector() {
    // Зупиняємо потік
    running = false;
    cv.notify_all();
    if (workerThread.joinable()) {
        workerThread.join();
    }
}

void FaceDetector::submitFrame(const cv::Mat& frame) {
    std::lock_guard<std::mutex> lock(mutex);
    pendingFrame = frame.clone();
    hasNewFrame = true;
    cv.notify_one();
}

std::vector<cv::Rect> FaceDetector::getDetections() {
    std::lock_guard<std::mutex> lock(mutex);
    return lastDetections;
}

bool FaceDetector::isLoaded() const {
    return loaded;
}

void FaceDetector::detectionLoop() {
    while (running) {
        cv::Mat frame;

        // Чекаємо новий кадр
        {
            std::unique_lock<std::mutex> lock(mutex);
            cv.wait(lock, [this] { return hasNewFrame || !running; });
            if (!running) break;
            frame = pendingFrame.clone();
            hasNewFrame = false;
        }

        // Детекція облич
        cv::Mat blob = cv::dnn::blobFromImage(frame, 1.0, cv::Size(300, 300),
                                               cv::Scalar(104.0, 177.0, 123.0));
        net.setInput(blob);
        cv::Mat detections = net.forward();

        // Парсимо результати
        std::vector<cv::Rect> faces;
        cv::Mat detMat(detections.size[2], detections.size[3], CV_32F,
                       detections.ptr<float>());

        for (int i = 0; i < detMat.rows; i++) {
            float confidence = detMat.at<float>(i, 2);
            if (confidence > 0.5f) {
                // Координати в відносних одиницях — перетворюємо в пікселі
                int x1 = (int)(detMat.at<float>(i, 3) * frame.cols);
                int y1 = (int)(detMat.at<float>(i, 4) * frame.rows);
                int x2 = (int)(detMat.at<float>(i, 5) * frame.cols);
                int y2 = (int)(detMat.at<float>(i, 6) * frame.rows);

                x1 = std::max(0, x1);
                y1 = std::max(0, y1);
                x2 = std::min(frame.cols - 1, x2);
                y2 = std::min(frame.rows - 1, y2);

                faces.push_back(cv::Rect(x1, y1, x2 - x1, y2 - y1));
            }
        }

        // Зберігаємо результати
        {
            std::lock_guard<std::mutex> lock(mutex);
            lastDetections = faces;
        }
    }
}