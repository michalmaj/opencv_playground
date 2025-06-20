//
// Created by Michał Maj on 20/06/2025.
//

#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <random>
#include <stdexcept>
#include <vector>

struct CoinDetection {
    cv::Mat src, gray;
    std::vector<cv::Mat> split;

    // Window name
    const std::string image_window{ "Processed Image" };
    const std::string thresh_window{ "Threshold Controls" };
    const std::string morph_window{ "Morphology Controls" };

    // Type of operations
    /*
     * 1. Thresholding
     * 2. Morphological Operations
     * 3. Blob Detection
     * 4. Contour Detection
     * 5. Connected Component Analysis
     */

     // Threshold
    cv::Mat thresholded;
    int threshold_type{ 0 };
    int threshold_steps{ 4 };
    int threshold_min_value{ 0 };
    int threshold_max_value{ 255 };
    int steps{ 255 };
    std::string number_threshold_types{ "Threshold Types" };
    std::string min_thresh{ "Threshold Min Value" };
    std::string max_thresh{ "Threshold Max Value" };

    // Morphological Operations
    cv::Mat morphed;
    int morph_type{ 0 };
    int morph_steps{ 3 };
    int kernel_morph_type{ 0 };
    int kernel_morph_steps{ 2 };
    int kernel_multiplier{ 0 };
    int kernel_multiplier_steps{ 10 };
    int kernel_size{ 3 + kernel_multiplier * 2 };
    int number_of_iterations{ 1 };
    int how_many_iterations{ 20 };
    std::string number_morph_types{ "Morph Types" };
    std::string number_kernel_morph_types{ "Kernel Types" };
    std::string number_kernel_steps{ "Kernel Steps" };
    std::string numer_of_iterations{ "Number of iterations" };
};

void thresholdImage(CoinDetection& cd) {
    cv::threshold(cd.gray, cd.thresholded, cd.threshold_min_value, cd.threshold_max_value, cd.threshold_type);
}

void ProcessThreshold(int, void* data) {
    auto* cd = static_cast<CoinDetection*>(data);
    thresholdImage(*cd);
    cv::imshow(cd->thresh_window, cd->thresholded);
}

void morphImage(CoinDetection& cd) {
    auto element{ cv::getStructuringElement(cd.kernel_morph_type, cv::Size(cd.kernel_size, cd.kernel_size)) };
    if (cd.morph_type == 0) {
        cv::erode(cd.thresholded, cd.morphed, element, cv::Point(-1, -1), cd.number_of_iterations);
    }
    else if (cd.morph_type == 1) {
        cv::dilate(cd.thresholded, cd.morphed, element, cv::Point(-1, -1), cd.number_of_iterations);
    }
    else if (cd.morph_type == 2) {
        cv::morphologyEx(cd.thresholded, cd.morphed, cv::MORPH_CLOSE, element, cv::Point(-1, -1), cd.number_of_iterations);
    }
    else if (cd.morph_type == 3) {
        cv::morphologyEx(cd.thresholded, cd.morphed, cv::MORPH_OPEN, element, cv::Point(-1, -1), cd.number_of_iterations);
    }
}

void ProcessMorph(int, void* data) {
    auto* cd = static_cast<CoinDetection*>(data);
    morphImage(*cd);
    cv::imshow(cd->morph_window, cd->morphed);
}

int main() {
    // Path to an image
    std::string path{ "../data/images/CoinsA.png" };

    // Load an image
    cv::Mat img{ cv::imread(path) };

    // Check if image is loaded
    try {
        if (img.empty()) {
            throw std::runtime_error(std::format("Can't load image from {}", path));
        }
    }
    catch (std::exception& e) {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    CoinDetection cd;
    cd.src = img.clone();
    cv::split(cd.src, cd.split);
    cv::cvtColor(cd.src, cd.gray, cv::COLOR_BGR2GRAY);
    cd.thresholded = cd.gray.clone();

    // Different windows
    cv::namedWindow(cd.image_window, cv::WINDOW_AUTOSIZE);
    cv::namedWindow(cd.thresh_window, cv::WINDOW_AUTOSIZE);
    cv::namedWindow(cd.morph_window, cv::WINDOW_AUTOSIZE);

    // Create trackbars for thresholding
    cv::createTrackbar(cd.number_threshold_types, cd.thresh_window, &cd.threshold_type, cd.threshold_steps, ProcessThreshold, &cd);
    cv::createTrackbar(cd.min_thresh, cd.thresh_window, &cd.threshold_min_value, cd.steps, ProcessThreshold, &cd);
    cv::createTrackbar(cd.max_thresh, cd.thresh_window, &cd.threshold_max_value, cd.steps, ProcessThreshold, &cd);

    // Create trackbars for Morphological Operations
    cv::createTrackbar(cd.number_morph_types, cd.morph_window, &cd.morph_type, cd.morph_steps, ProcessMorph, &cd);
    cv::createTrackbar(cd.number_kernel_morph_types, cd.morph_window, &cd.kernel_morph_type, cd.kernel_morph_steps, ProcessMorph, &cd);
    cv::createTrackbar(cd.number_kernel_steps, cd.morph_window, &cd.kernel_multiplier, cd.kernel_multiplier_steps, ProcessMorph, &cd);
    cv::createTrackbar(cd.numer_of_iterations, cd.morph_window, &cd.number_of_iterations, cd.how_many_iterations, ProcessMorph, &cd);

    // Initial windo
    cv::imshow(cd.image_window, cd.src);

    while (true) {
        auto key{ cv::waitKey(10) };

        // Quit the program
        if (key == 'q') {
            break;
        }

        // Reset permanently image
        if (key == 'c') {
            cd.thresholded = cd.gray.clone();
            cv::imshow(cd.image_window, cd.gray);
        }
        else if (key == 'b') {
            cd.thresholded = cd.split.at(0).clone();
            cv::imshow(cd.image_window, cd.split.at(0));
        }
        else if (key == 'g') {
            cd.thresholded = cd.split.at(1).clone();
            cv::imshow(cd.image_window, cd.split.at(1));
        }
        else if (key == 'r') {
            cd.thresholded = cd.split.at(2).clone();
            cv::imshow(cd.image_window, cd.split.at(2));
        }
    }

    cv::destroyAllWindows();

    return 0;
}