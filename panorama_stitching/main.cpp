//
// Created by Michał Maj on 21/06/2025.
//

#include <filesystem>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/photo.hpp>
#include <opencv2/stitching.hpp>
#include <print>
#include <ranges>
#include <unordered_set>

class Stitcher {
public:
    Stitcher(const std::filesystem::path& path, const cv::Stitcher::Mode& mode = cv::Stitcher::Mode::PANORAMA)
        : mode_(mode) {
        auto len{ std::distance(std::filesystem::directory_iterator(path), std::filesystem::directory_iterator{}) };
        images_.reserve(len);

        if (!std::filesystem::exists(path) or !std::filesystem::is_directory(path)) {
            throw std::runtime_error(std::format("Invalid directory path: {}", path.string()));
        }
        for (const auto& e : std::filesystem::directory_iterator(path)) {
            if (!std::filesystem::is_regular_file(e)) {
                continue;
            }
            std::string ext{ e.path().extension().string() };
            if (!valid_extensions_.contains(ext)) {
                continue;
            }
            cv::Mat img{ cv::imread(e.path().string()) };
            if (img.empty()) {
                std::cerr << std::format("Failed to load an image from: {}\n", e.path().string());
                continue;
            }
            images_.emplace_back(std::move(img));
        }

        if (images_.size() < 2) {
            throw std::runtime_error("Stitcher requires at least 2 images!\n");
        }
    }

    void setMode(const cv::Stitcher::Mode& mode) {
        mode_ = mode;
    }

    void setNewImages(const std::filesystem::path& path) {
        if (!std::filesystem::exists(path) or !std::filesystem::is_directory(path)) {
            std::cout << std::format("Invalid path: {}\n", path.string());
            return;
        }

        std::vector<cv::Mat> temp;
        for (const auto& e : std::filesystem::directory_iterator(path)) {
            if (!std::filesystem::is_regular_file(e)) {
                continue;
            }
            std::string ext{ e.path().extension().string() };

            if (!valid_extensions_.contains(ext)) {
                continue;
            }

            cv::Mat img{ cv::imread(e.path().string()) };
            if (img.empty()) {
                std::cerr << std::format("Can't load an image from: {}", e.path().string());
                continue;
            }
            temp.emplace_back(std::move(img));
        }
        if (temp.size() < 2) {
            std::cerr << "Not enough images, Stitcher requires at least 2 images!\n";
            return;
        }
        std::swap(temp, images_);
    }

    std::optional<cv::Mat> createStitcher() {
        cv::Mat output;
        stitcher_ = cv::Stitcher::create(mode_);
        auto status{ stitcher_->stitch(images_, output) };

        if (status != cv::Stitcher::OK) {
            std::cerr << "Can't stitch images!\n";
            return std::nullopt;
        }

        return output;
    }

private:
    std::vector<cv::Mat> images_;
    cv::Stitcher::Mode mode_;
    cv::Ptr<cv::Stitcher> stitcher_;
    static inline std::unordered_set<std::string> valid_extensions_{ ".jpg", ".jpeg", ".png" };
};

int main() {
    std::filesystem::path path{ "../data/images/scene" };
    Stitcher s{ path };

    std::string win_name{ "Stitched" };
    cv::namedWindow(win_name, cv::WINDOW_NORMAL);
    auto stiched{ s.createStitcher() };
    if (stiched.has_value()) {
        cv::imshow(win_name, stiched.value());
    }

    cv::waitKey(0);
    cv::destroyAllWindows();

    return 0;
}