//
// Created by Michał Maj on 21/06/2025.
//

#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/photo.hpp>
#include <stdexcept>
#include <format>
#include <filesystem>
#include <vector>

class ColorPatchSelector {
public:
    void setLab(const cv::Mat& bgr) {
        if (bgr.type() != CV_8UC3) {
            throw std::runtime_error("Wrong image type!\n");
        }
        cv::cvtColor(bgr, lab_, cv::COLOR_BGR2Lab);
    }

    void addColor(const cv::Point& p) {
        if (lab_.empty()) {
            throw std::runtime_error("There is no LAB image!\n");
        }
        colors_.push_back(lab_.at<cv::Vec3b>(p.y, p.x));
    }

    std::optional<cv::Mat> getMask() {
        if (colors_.empty()) {
            return std::nullopt;
        }
        cv::Mat mask;
        findLowerUpper();
        cv::inRange(lab_, lower_, upper_, mask);
        return mask;
    }

private:
    cv::Mat lab_;
    std::vector<cv::Vec3b> colors_;
    cv::Scalar lower_{ 255, 255, 255 };
    cv::Scalar upper_{ 0, 0, 0 };

    void findLowerUpper() {
        std::ranges::for_each(colors_, [this](const auto& color) {
            lower_[0] = std::min(static_cast<int>(color[0]), static_cast<int>(lower_[0]));
            lower_[1] = std::min(static_cast<int>(color[1]), static_cast<int>(lower_[1]));
            lower_[2] = std::min(static_cast<int>(color[2]), static_cast<int>(lower_[2]));

            upper_[0] = std::max(static_cast<int>(color[0]), static_cast<int>(upper_[0]));
            upper_[1] = std::max(static_cast<int>(color[1]), static_cast<int>(upper_[1]));
            upper_[2] = std::max(static_cast<int>(color[2]), static_cast<int>(upper_[2]));
            });
    }
};

class ScreenMatting {
public:
    ScreenMatting(const std::filesystem::path& background_path) : background_(cv::imread(background_path.string())) {}

    auto& getImg() { return img_; }
    auto& getMask() const { return mask_; }
    auto& getBackground() const { return background_; }

    auto& getImageWindowName() const {
        return image_window_name_;
    }

    void convertToLab() {
        cps_.setLab(img_);
    }

    void setPoints(const cv::Point& p) {
        cps_.addColor(p);
    }

    void setMask() {
        auto maskOpt = cps_.getMask();
        if (!maskOpt) {
            return;
        }
        mask_ = std::move(*maskOpt);
    }

    void process() {
        if (mask_.empty()) {
            return;
        }
        cv::bitwise_not(mask_, mask_);
        softness();
        cv::cvtColor(mask_, mask_, cv::COLOR_GRAY2BGR);
        if (!resized) {
            resizeBackground();
            resized = true;
        }
        cv::Mat temp = background_.clone();
        cv::bitwise_and(temp, ~mask_, temp);
        cv::bitwise_and(img_, mask_, img_);
        cv::bitwise_or(img_, temp, img_);
    }

    // Softness setup
    int blur_idx{ 0 };
    static constexpr int max_blur{ 11 };

    int erode_idx{ 0 };
    static constexpr int max_erode{ 11 };

    static constexpr std::array<int, max_blur> kernels{ 0, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21 };

private:
    cv::Mat img_;
    cv::Mat mask_;
    cv::Mat background_;
    ColorPatchSelector cps_;
    bool resized{ false };

    const std::string image_window_name_{ "Original Image" };

    void resizeBackground() {
        cv::resize(background_, background_, img_.size());
    }

    void softness() {
        if (mask_.empty()) {
            return;
        }
        if (blur_idx == 0) {
            return;
        }
        cv::blur(mask_, mask_, cv::Size(kernels.at(blur_idx), kernels.at(blur_idx)));

        if (erode_idx == 0) {
            return;
        }
        auto element = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(kernels.at(erode_idx), kernels.at(erode_idx)));
        cv::erode(mask_, mask_, element);
    }
};

void colorSelector(int event, int x, int y, int flags, void* data) {
    ScreenMatting* sm = static_cast<ScreenMatting*>(data);

    if (event == cv::EVENT_LBUTTONDOWN) {
        sm->setPoints({ x, y });
    }
}

int main() {
    cv::VideoCapture cap;
    std::filesystem::path video_path{ "../data/videos/greenscreen-asteroid.mp4" };
    cap.open(video_path.string());
    if (!cap.isOpened()) {
        std::cerr << std::format("Can't load video from: {}\n", video_path.string());
        return EXIT_FAILURE;
    }

    std::filesystem::path path{ "../data/images/IF4.png" };
    if (!std::filesystem::exists(path)) {
        std::cerr << std::format("Can't load an image from: {}", path.string());
        return EXIT_FAILURE;
    }
    ScreenMatting sm{ path };

    cv::namedWindow(sm.getImageWindowName(), cv::WINDOW_AUTOSIZE);
    cv::setMouseCallback(sm.getImageWindowName(), colorSelector, &sm);
    cv::createTrackbar("Blur", sm.getImageWindowName(), &sm.blur_idx, sm.max_blur - 1);
    cv::createTrackbar("Erode", sm.getImageWindowName(), &sm.erode_idx, sm.max_erode - 1);

    while (true) {
        cap.read(sm.getImg());
        if (sm.getImg().empty()) {
            break;
        }
        sm.convertToLab();
        sm.setMask();

        auto c = cv::waitKey(100);
        if (c == 'q') {
            break;
        }

        sm.process();

        if (!sm.getMask().empty()) {
            cv::imshow("Mask", sm.getMask());
        }

        cv::imshow(sm.getImageWindowName(), sm.getImg());
        cv::imshow("Background", sm.getBackground());
    }

    cap.release();
    cv::destroyAllWindows();


    return 0;
}