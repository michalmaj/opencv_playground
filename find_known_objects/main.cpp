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

class FindKnownObjects {
public:
    FindKnownObjects(
        const std::filesystem::path& src_path,
        const std::filesystem::path& dst_path,
        int max_features = 1000,
        int min_mach_count = 10,
        int trees_number = 5,
        int number_of_checks = 5,
        float lowe_ratio = 0.9f,
        float ransac_threshold = 5.0f
    ) :
        src_(cv::imread(src_path.string())),
        dst_(cv::imread(dst_path.string())),
        max_features_(max_features),
        min_match_count_(min_mach_count),
        trees_number_(trees_number),
        number_of_checks_(number_of_checks),
        lowe_ratio_(lowe_ratio),
        ransac_threshold_(ransac_threshold) {
        if (src_.empty()) {
            throw std::runtime_error(std::format("Can't load an image from: {}", src_path.string()));
        }

        if (dst_.empty()) {
            throw std::runtime_error(std::format("Can't load an image from: {}", dst_path.string()));
        }

        process();
    }

    cv::Mat drawMatches(bool use_mask = true,
        bool draw_location = true,
        const cv::DrawMatchesFlags& flag = cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS) {
        cv::Mat output;
        cv::Mat mask{};

        if (use_mask) {
            mask = mask_;
        }
        cv::drawMatches(
            src_,
            kp1_,
            dst_,
            kp2_,
            good_matches_,
            output,
            cv::Scalar::all(-1),
            cv::Scalar::all(-1),
            mask,
            flag
        );

        if (draw_location) {
            drawLines(output);
        }

        return output;
    }
private:
    // Matrices for images
    cv::Mat src_;
    cv::Mat dst_;

    // Matrices for descriptors
    cv::Mat descriptors1_;
    cv::Mat descriptors2_;

    // Vectors for keypoints
    std::vector<cv::KeyPoint> kp1_;
    std::vector<cv::KeyPoint> kp2_;

    // Matrices for homography and mask
    cv::Mat homography_;
    cv::Mat mask_;

    // Vectors for homography
    std::vector<cv::Point2f> src_points_;
    std::vector<cv::Point2f> dst_points_;

    // Vectors for matches and for good matches
    std::vector<std::vector<cv::DMatch>> matches_;
    std::vector<cv::DMatch> good_matches_;

    // Configuration variables
    int max_features_{};
    int min_match_count_{};
    int trees_number_{};
    int number_of_checks_{};
    float lowe_ratio_{};
    float ransac_threshold_{};

    void process() {
        findKeyPointsAndDescriptors();
        matchFeatures();
        findGoodMatches();
        auto check{ fillSrcDst() };
        if (!check) {
            return;
        }
        findHomography();
    }

    void findKeyPointsAndDescriptors() {
        cv::Mat gray_src, gray_dst;
        cv::cvtColor(src_, gray_src, cv::COLOR_BGR2GRAY);
        cv::cvtColor(dst_, gray_dst, cv::COLOR_BGR2GRAY);

        cv::Ptr<cv::ORB> orb{ cv::ORB::create(max_features_) };
        orb->detectAndCompute(gray_src, cv::Mat{}, kp1_, descriptors1_);
        orb->detectAndCompute(gray_dst, cv::Mat{}, kp2_, descriptors2_);
    }

    void matchFeatures() {
        descriptors1_.convertTo(descriptors1_, CV_32F);
        descriptors2_.convertTo(descriptors2_, CV_32F);

        cv::FlannBasedMatcher macher{
          new cv::flann::KDTreeIndexParams(trees_number_),
          new cv::flann::SearchParams(number_of_checks_)
        };
        macher.knnMatch(descriptors1_, descriptors2_, matches_, 2);
    }

    void findGoodMatches() {
        good_matches_ = matches_ |
            std::views::filter([this](const auto& e) {return e[0].distance < lowe_ratio_ * e[1].distance; }) |
            std::views::transform([](const auto& e) {return e[0]; }) |
            std::ranges::to<std::vector>();
    }

    bool fillSrcDst() {
        if (good_matches_.size() < min_match_count_) {
            return false;
        }

        src_points_.reserve(good_matches_.size());
        dst_points_.reserve(good_matches_.size());

        std::ranges::for_each(good_matches_, [this](const auto& e) {
            src_points_.emplace_back(kp1_[e.queryIdx].pt);
            dst_points_.emplace_back(kp2_[e.trainIdx].pt);
            });

        return true;
    }

    void findHomography() {
        homography_ = cv::findHomography(src_points_, dst_points_, cv::RANSAC, ransac_threshold_, mask_);
    }

    void drawLines(cv::Mat& img) {
        std::vector<cv::Point2f> src_corners{
          {0.0f, 0.0f},
          {static_cast<float>(src_.size().width), 0.0f},
          {static_cast<float>(src_.size().width), static_cast<float>(src_.size().height)},
          {0.0f, static_cast<float>(src_.size().height)}
        };
        std::vector<cv::Point2f> scene_corners(4);
        cv::perspectiveTransform(src_corners, scene_corners, homography_);

        float offset_x = static_cast<float>(src_.size().width);
        for (auto& pt : scene_corners) {
            pt.x += offset_x;
        }

        std::vector<cv::Point2f> hull;
        cv::convexHull(scene_corners, hull);
        for (size_t i{ 0 }; i < hull.size(); ++i) {
            cv::line(img,
                hull[i],
                hull[(i + 1) % hull.size()],
                cv::Scalar(0, 0, 255),
                5,
                cv::LINE_AA);
        }
    }
};

int main() {
    std::filesystem::path path1{ "../data/images/book.png" };
    if (!std::filesystem::exists(path1)) {
        std::cerr << std::format("Can't find file at given location: {}", path1.string());
        return EXIT_FAILURE;
    }

    std::filesystem::path path2{ "../data/images/book_scene.png" };
    if (!std::filesystem::exists(path2)) {
        std::cerr << std::format("Can't find file at given location: {}", path2.string());
        return EXIT_FAILURE;
    }

    FindKnownObjects o{ path1, path2 };

    cv::namedWindow("Matched", cv::WINDOW_NORMAL);

    cv::imshow("Matched", o.drawMatches());
    cv::waitKey(0);
    cv::destroyAllWindows();


    return 0;
}