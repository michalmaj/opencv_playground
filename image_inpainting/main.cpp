//
// Created by Michał Maj on 20/06/2025.
//

#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/photo.hpp>
#include <stdexcept>
#include <format>
#include <filesystem>
#include <array>

class ImageInpainting {
public:
    ImageInpainting(const std::string& path) : img_(cv::imread(path, cv::IMREAD_GRAYSCALE)) {
        if (img_.empty()) {
            throw std::runtime_error(std::format("Can't load an image from: {}", path));
        }
        copy_ = img_.clone();
        mask_ = cv::Mat::zeros(img_.size(), CV_8U);
    }

    cv::Mat process(int flag) {
        cv::Mat processed_;
        cv::inpaint(img_, mask_, processed_, 3, flag);

        return processed_;
    }

    const cv::Mat& getImg() const {
        return img_;
    }

    void setImg(const cv::Mat& img) {
        img_ = img.clone();
    }

    cv::Mat& getMask() {
        return mask_;
    }

    const cv::Mat& getCopy() const {
        return copy_;
    }

    const std::string& getOriginalName() const {
        return original_name_;
    }

    const std::string& getMaskName() const {
        return mask_name_;
    }

    int& getLineIdx() {
        return line_idx;
    }

    const int& getMaxLineIdx() const {
        return max_line_idx;
    }

    const int& getTypeOfLine(int idx) {
        return type_of_line.at(idx);
    }

    int& getLineThickness() {
        line_thickness = std::clamp(line_thickness, 1, max_line_thickness);
        return line_thickness;
    }

    const int& getMaxLineThickness() const {
        return max_line_thickness;
    }

    void setPreviousPoint(const cv::Point& point) {
        previous_point_ = point;
    }

    cv::Point getPreviousPoint() const {
        return previous_point_;
    }

private:
    // Matrices for images
    cv::Mat img_;
    cv::Mat mask_;
    cv::Mat copy_;

    // Point for drawing purpose
    cv::Point previous_point_{ -1, -1 };

    // Windows names
    const std::string original_name_{ "Original Image" };
    const std::string mask_name_{ "Mask" };

    // Line type
    static constexpr std::array<int, 3> type_of_line{ cv::LINE_4 , cv::LINE_8 , cv::LINE_AA };
    int line_idx{ 0 };
    int max_line_idx{ type_of_line.size() - 1 };

    // Line thickness
    int line_thickness{ 1 };
    const int max_line_thickness{ 20 };
};

void drawLine(int event, int x, int y, int flags, void* data) {
    ImageInpainting* ii = static_cast<ImageInpainting*>(data);

    if (!ii) {
        std::cerr << "Error: ImageInpainting pointer is null!" << std::endl;
        return;
    }

    if (event == cv::EVENT_LBUTTONUP || !(flags & cv::EVENT_FLAG_LBUTTON)) {
        ii->setPreviousPoint({ -1, -1 });
    }
    else if (event == cv::EVENT_LBUTTONDOWN) {
        ii->setPreviousPoint({ x, y });
    }
    else if (event == cv::EVENT_MOUSEMOVE && (flags & cv::EVENT_FLAG_LBUTTON)) {
        cv::Point pt{ x, y };
        if (ii->getPreviousPoint().x < 0) {
            ii->setPreviousPoint(pt);
        }

        // Draw a white line on mask
        cv::line(ii->getMask(),
            ii->getPreviousPoint(),
            pt, cv::Scalar::all(255),
            ii->getLineThickness(),
            ii->getTypeOfLine(ii->getLineIdx()));

        // Draw a white line on image
        cv::line(ii->getImg(),
            ii->getPreviousPoint(),
            pt, cv::Scalar::all(255),
            ii->getLineThickness(),
            ii->getTypeOfLine(ii->getLineIdx()));

        ii->setPreviousPoint(pt);

        cv::imshow(ii->getOriginalName(), ii->getImg());
        cv::imshow(ii->getMaskName(), ii->getMask());
    }
}

int main() {
    // Set paths to images
    std::string path{ "../data/images/Lincoln.png" };

    ImageInpainting ii{ path };
    cv::namedWindow(ii.getOriginalName(), cv::WINDOW_AUTOSIZE);

    cv::imshow(ii.getOriginalName(), ii.getImg());
    cv::createTrackbar("Line Thickness",
        ii.getOriginalName(),
        &ii.getLineThickness(),
        ii.getMaxLineThickness(),
        nullptr,
        &ii);
    cv::createTrackbar("Line Type",
        ii.getOriginalName(),
        &ii.getLineIdx(),
        ii.getMaxLineIdx(),
        nullptr,
        &ii);
    cv::setMouseCallback(ii.getOriginalName(), drawLine, &ii);
    while (true) {
        auto c = cv::waitKey(0);

        if (c == 'q') {
            break;
        }

        if (c == 'n') {
            auto result = ii.process(cv::INPAINT_NS);
            cv::imshow("Navier-Stokes based method", result);
        }

        if (c == 't') {
            auto result = ii.process(cv::INPAINT_TELEA);
            cv::imshow("Fast marching based method", result);
        }

        if (c == 'r') {
            ii.getMask() = cv::Scalar::all(0);
            ii.setImg(ii.getCopy());
            try {
                cv::destroyWindow("Navier-Stokes based method");
                cv::destroyWindow("Fast marching based method");
            }
            catch (std::exception& e) {
                std::cerr << std::format("Windows don't exist: {}", e.what());
            }
            cv::imshow(ii.getOriginalName(), ii.getImg());
        }

    }

    cv::destroyAllWindows();
    return 0;
}

