//
// Created by Michał Maj on 21/06/2025.
//

#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/photo.hpp>
#include <filesystem>
#include <print>
#include <ranges>

class Homography {
public:
    Homography(const std::filesystem::path& src, const std::filesystem::path& dst)
        : src_(cv::imread(src.string())), dst_(cv::imread(dst.string())) {
        src_points_.reserve(4);
        dst_points_.reserve(4);
    }
    Homography(const std::filesystem::path& src) : src_(cv::imread(src.string())) {
        dst_ = cv::Mat::zeros(src_.size(), src_.type());
        src_points_.reserve(4);
        dst_points_.reserve(4);
    }

    const auto& getSrc() const { return src_; }
    const auto& getDst() const { return dst_; }
    std::optional<cv::Mat> getWarped() const {
        if (warped_.empty()) {
            return std::nullopt;
        }
        return warped_;
    }

    auto getSrcPointsSize() const {
        return src_points_.size();
    }

    auto getDstPointsSize() const {
        return dst_points_.size();
    }

    constexpr std::string srcWindowName() const {
        return "Src";
    }
    constexpr std::string dstWindowName() const {
        return "Dst";
    }

    void setSrcPoints(cv::Point p) {
        if (src_points_.size() >= 4) {
            std::println("You have already 4 source points");
            return;
        }
        src_points_.emplace_back(p);
    }

    void setDstPoints(cv::Point p) {
        if (dst_points_.size() >= 4) {
            std::println("You have already 4 destination points");
            return;
        }
        dst_points_.emplace_back(p);
    }

    void process(bool source_to_dst = false) {
        if (src_points_.size() != 4) {
            emptySrc();
        }

        sort_points(src_points_);

        if (dst_.empty() or dst_points_.size() != 4) {
            emptyDst();
        }
        else {
            sort_points(dst_points_);
            dst_size_ = dst_.size();
        }

        auto h = cv::findHomography(src_points_, dst_points_);

        cv::warpPerspective(src_, warped_, h, dst_size_);

        if (source_to_dst) {
            cv::Mat mask = cv::Mat::zeros(dst_.size(), dst_.type());
            cv::fillConvexPoly(mask, dst_points_, cv::Scalar(255, 255, 255));

            dst_.setTo(cv::Scalar(0, 0, 0), mask);
            cv::add(dst_, warped_, dst_);
        }
    }

    void reset() {
        src_points_.clear();
        dst_points_.clear();
    }


private:
    cv::Mat src_;
    cv::Mat dst_;
    cv::Mat warped_;
    cv::Size dst_size_;

    std::vector<cv::Point> src_points_;
    std::vector<cv::Point> dst_points_;

    void sort_points(std::vector<cv::Point>& points) {
        if (points.size() != 4) return;

        cv::Point topLeft = *std::min_element(points.begin(), points.end(),
            [](const cv::Point& a, const cv::Point& b) { return (a.x + a.y) < (b.x + b.y); });

        cv::Point bottomRight = *std::max_element(points.begin(), points.end(),
            [](const cv::Point& a, const cv::Point& b) { return (a.x + a.y) < (b.x + b.y); });

        cv::Point topRight = *std::min_element(points.begin(), points.end(),
            [](const cv::Point& a, const cv::Point& b) { return (a.x - a.y) > (b.x - b.y); });

        cv::Point bottomLeft = *std::max_element(points.begin(), points.end(),
            [](const cv::Point& a, const cv::Point& b) { return (a.x - a.y) > (b.x - b.y); });

        points = { topLeft, topRight, bottomRight, bottomLeft };
    }


    void emptyDst() {
        cv::Rect bbox = cv::boundingRect(src_points_);
        dst_size_ = bbox.size();

        dst_points_ = {
            {0, 0},
            {dst_size_.width - 1, 0},
            {dst_size_.width - 1, dst_size_.height - 1},
            {0, dst_size_.height - 1}
        };
    }

    void emptySrc() {
        src_points_ = {
          {0, 0},
          {src_.size().width - 1, 0},
          {src_.size().width - 1, src_.size().height - 1},
          {0, src_.size().height - 1}
        };
    }

};

void setSrcPoints(int event, int x, int y, int flag, void* data) {
    Homography* h = static_cast<Homography*>(data);
    if (event == cv::EVENT_LBUTTONDOWN) {
        if (h->getSrcPointsSize() < 4) {
            h->setSrcPoints({ x, y });
        }
    }
}

void setDstPoints(int event, int x, int y, int flag, void* data) {
    Homography* h = static_cast<Homography*>(data);
    if (event == cv::EVENT_LBUTTONDOWN) {
        if (h->getDstPointsSize() < 4) {
            h->setDstPoints({ x, y });
        }
    }
}

int main() {
    std::filesystem::path book2_path{ "../data/images/first-image.png" };
    std::filesystem::path book1_path{ "../data/images/times-square.png" };

    if (!std::filesystem::exists(book2_path)) {
        std::cerr << std::format("Can't load an image from: {}", book2_path.string());
        return EXIT_FAILURE;
    }

    if (!std::filesystem::exists(book1_path)) {
        std::cerr << std::format("Can't load an image from: {}", book1_path.string());
        return EXIT_FAILURE;
    }

    Homography h1{ book2_path, book1_path };


    cv::namedWindow(h1.srcWindowName(), cv::WINDOW_AUTOSIZE);
    cv::namedWindow(h1.dstWindowName(), cv::WINDOW_AUTOSIZE);

    cv::setMouseCallback(h1.srcWindowName(), setSrcPoints, &h1);
    cv::setMouseCallback(h1.dstWindowName(), setDstPoints, &h1);

    while (true) {
        auto c = cv::waitKey(10);
        if (c == 'q') {
            break;
        }

        if (c == 'p') {
            h1.process();
        }

        if (c == 'f') {
            h1.process(true);
        }

        if (c == 'r') {
            h1.reset();
        }

        cv::imshow(h1.srcWindowName(), h1.getSrc());
        cv::imshow(h1.dstWindowName(), h1.getDst());
        if (h1.getWarped().has_value()) {
            cv::imshow("Warped", h1.getWarped().value());
        }
    }

    cv::destroyAllWindows();

    return 0;
}