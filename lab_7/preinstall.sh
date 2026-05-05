#!/bin/bash
sudo apt-get update --fix-missing
sudo apt-get install -y libopencv-dev cmake g++

# Завантажуємо файли нейронної мережі ResNet-10
wget -q --show-progress -O deploy.prototxt \
    https://raw.githubusercontent.com/opencv/opencv/master/samples/dnn/face_detector/deploy.prototxt

wget -q --show-progress -O res10_300x300_ssd_iter_140000.caffemodel \
    https://raw.githubusercontent.com/opencv/opencv_3rdparty/dnn_samples_face_detector_20170830/res10_300x300_ssd_iter_140000.caffemodel