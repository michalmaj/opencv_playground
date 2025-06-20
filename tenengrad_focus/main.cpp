//
// Created by Michał Maj on 20/06/2025.
//

#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <stdexcept>

auto calculateTenengradFocus(const cv::Mat& img) {
    if (img.channels() > 1) {
        throw std::runtime_error("This function requires grayscale image!\n");
    }

    cv::Mat gradient_x, gradient_y;

    // Calculate gradients in x and y directions
    cv::Sobel(img, gradient_x, CV_64F, 1, 0);
    cv::Sobel(img, gradient_y, CV_64F, 0, 1);

    // Square both gradients and sum them up
    cv::Mat gradient_magnitude = gradient_x.mul(gradient_x) + gradient_y.mul(gradient_y);

    // Return sum of all pixels
    return cv::sum(gradient_magnitude)[0];
}

int main() {
    // Path to video
    std::string path{ "../data/videos/focus-test.mp4" };

    // Capture video from given path
    cv::VideoCapture cap(path);

    // Check if video is loaded
    try {
        if (!cap.isOpened()) {
            throw std::runtime_error(std::format("Can't load video from {}", path));
        }
    }
    catch (std::exception& e) {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    // Get width and height of video
    auto width{ static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH)) };
    auto height{ static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT)) };

    // Calculate how many pixels should be taken to calculate focus
    const double percent_pixels{ 10 / 100.0 };
    auto num_pixels_x{ static_cast<int>(width * percent_pixels) };
    auto num_pixels_y{ static_cast<int>(height * percent_pixels) };
    cv::Point middle{ width / 2, height / 2 };

    cv::Mat frame; // Place for frames from stream
    cv::Mat best; // Place for frame with best focus
    double sum{};
    while (true) {
        cap.read(frame);
        if (frame.empty()) {
            break;
        }

        // Get roi based on middle point and percentage of pixels
        cv::Mat roi{ frame(cv::Range(middle.y - num_pixels_y, middle.y + num_pixels_y),
            cv::Range(middle.x - num_pixels_x, middle.x + num_pixels_x)).clone() };

        // Change from BGR to gray for calculations
        cv::cvtColor(roi, roi, cv::COLOR_BGR2GRAY);

        // Calculate sum for every frame based on this paper: https://www.researchgate.net/publication/3887632_Diatom_autofocusing_in_brightfield_microscopy_A_comparative_study
        auto current_sum{ calculateTenengradFocus(roi) };
        if (current_sum > sum) {
            best = frame.clone();
            sum = current_sum;
        }
        cv::imshow("Original", frame);
        cv::imshow("ROI", roi);
        cv::waitKey(25);
    }

    cv::imshow("Best", best);
    cv::waitKey(0);
    cap.release();
    cv::destroyAllWindows();
    return 0;
}
