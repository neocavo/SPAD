#include "CameraProvider.hpp"
#include "KeyProcessor.hpp"
#include "FrameProcessor.hpp"
#include "Display.hpp"
#include <iostream>

int main() {
    CameraProvider camera(0);
    if (!camera.isOpened()) {
        std::cerr << "Не вдалось відкрити камеру!" << std::endl;
        return -1;
    }

    KeyProcessor keyProcessor;
    FrameProcessor frameProcessor;
    Display display("Lab 6 - OpenCV");

    std::cout << "Керування:" << std::endl;
    std::cout << "N - нормальний режим" << std::endl;
    std::cout << "I - інверсія кольорів" << std::endl;
    std::cout << "B - Gaussian blur" << std::endl;
    std::cout << "C - Canny фільтр" << std::endl;
    std::cout << "S - фільтр Собеля" << std::endl;
    std::cout << "G - відтінки сірого" << std::endl;
    std::cout << "ESC - вихід" << std::endl;
    std::cout << "ЛКМ - намалювати кружок" << std::endl;
    std::cout << "ПКМ - прибрати кружок" << std::endl;

    while (true) {
        cv::Mat frame = camera.getFrame();
        if (frame.empty()) {
            std::cerr << "Порожній кадр!" << std::endl;
            break;
        }

        // Обробляємо кадр відповідно до поточного режиму
        cv::Mat processed = frameProcessor.process(frame, keyProcessor.getMode());

        // Відображаємо результат
        display.show(processed);

        // Обробляємо натискання клавіші
        int key = cv::waitKey(30);
        if (!keyProcessor.processKey(key)) {
            break;
        }
    }

    cv::destroyAllWindows();
    return 0;
}