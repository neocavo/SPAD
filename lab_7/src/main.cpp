#include "CameraProvider.hpp"
#include "KeyProcessor.hpp"
#include "FrameProcessor.hpp"
#include "Display.hpp"
#include "FaceDetector.hpp"
#include <iostream>
#include <chrono>

int main() {
    CameraProvider camera(0);
    if (!camera.isOpened()) {
        std::cerr << "Не вдалось відкрити камеру!" << std::endl;
        return -1;
    }

    KeyProcessor keyProcessor;
    FrameProcessor frameProcessor;
    Display display("Lab 7 - OpenCV + DNN");

    // Завантажуємо детектор облич
    FaceDetector faceDetector("deploy.prototxt",
                              "res10_300x300_ssd_iter_140000.caffemodel");

    if (!faceDetector.isLoaded()) {
        std::cerr << "Увага: не вдалось завантажити нейронну мережу!" << std::endl;
        std::cerr << "Режим FACE буде недоступний." << std::endl;
    }

    std::cout << "Керування:" << std::endl;
    std::cout << "N - нормальний режим" << std::endl;
    std::cout << "I - інверсія кольорів" << std::endl;
    std::cout << "B - Gaussian blur" << std::endl;
    std::cout << "C - Canny фільтр" << std::endl;
    std::cout << "S - фільтр Собеля" << std::endl;
    std::cout << "G - відтінки сірого" << std::endl;
    std::cout << "F - детекція облич (DNN)" << std::endl;
    std::cout << "ЛКМ - намалювати кружок" << std::endl;
    std::cout << "ПКМ - прибрати кружок" << std::endl;
    std::cout << "ESC - вихід" << std::endl;

    // FPS лічильник
    int frameCount = 0;
    double fps = 0.0;
    auto startTime = std::chrono::steady_clock::now();

    while (true) {
        cv::Mat frame = camera.getFrame();
        if (frame.empty()) {
            std::cerr << "Порожній кадр!" << std::endl;
            break;
        }

        Mode currentMode = keyProcessor.getMode();
        cv::Mat processed;

        if (currentMode == Mode::FACE) {
            // Відправляємо кадр в окремий потік для детекції
            faceDetector.submitFrame(frame);

            // Отримуємо останні результати (не блокуємо основний потік)
            processed = frame.clone();
            std::vector<cv::Rect> faces = faceDetector.getDetections();

            // Малюємо рамки навколо облич
            for (const auto& face : faces) {
                cv::rectangle(processed, face, cv::Scalar(0, 255, 0), 2);
                cv::putText(processed, "Face", cv::Point(face.x, face.y - 5),
                           cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 1);
            }

            // Показуємо кількість облич
            cv::putText(processed, "Mode: FACE DNN | Faces: " +
                       std::to_string(faces.size()),
                       cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX,
                       0.7, cv::Scalar(0, 255, 0), 2);
        } else {
            processed = frameProcessor.process(frame, currentMode);
        }

        // Рахуємо FPS
        frameCount++;
        auto now = std::chrono::steady_clock::now();
        double elapsed = std::chrono::duration<double>(now - startTime).count();
        if (elapsed >= 1.0) {
            fps = frameCount / elapsed;
            frameCount = 0;
            startTime = now;
        }

        // Відображаємо FPS на кадрі
        cv::putText(processed, "FPS: " + std::to_string((int)fps),
                    cv::Point(10, processed.rows - 10),
                    cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(255, 255, 0), 2);

        display.show(processed);

        int key = cv::waitKey(30);
        if (!keyProcessor.processKey(key)) {
            break;
        }
    }

    cv::destroyAllWindows();
    return 0;
}