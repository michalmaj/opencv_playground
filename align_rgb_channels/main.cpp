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

class FeatureMatching {
public:
    FeatureMatching(
        const std::filesystem::path& src_path,
        int max_features = 1000,
        int min_mach_count = 10,
        int trees_number = 5,
        int number_of_checks = 5,
        float lowe_ratio = 0.95f,
        float ransac_threshold = 5.0f
    ) :
        max_features_(max_features),
        min_match_count_(min_mach_count),
        trees_number_(trees_number),
        number_of_checks_(number_of_checks),
        lowe_ratio_(lowe_ratio),
        ransac_threshold_(ransac_threshold) {
        cv::Mat img{ cv::imread(src_path.string(), cv::IMREAD_GRAYSCALE) };

        splitImage(img);

        process();
    }

    cv::Mat drawMatches(bool use_mask = true,
        const cv::DrawMatchesFlags& flag = cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS) {
        cv::Mat output;
        cv::Mat mask{};

        if (use_mask) {
            mask = mask_;
        }
        cv::drawMatches(
            blue_,
            kp1_,
            green_,
            kp2_,
            good_matches_bg_,
            output,
            cv::Scalar::all(-1),
            cv::Scalar::all(-1),
            mask,
            flag
        );

        return output;
    }

    cv::Mat warpPerspective() {
        cv::Mat out;
        cv::warpPerspective(blue_, out, homography_bg_, green_.size());
        return out;
    }

    cv::Mat warpedImages() {
        std::vector<cv::Mat> v{ blue_warped_, green_, red_warped_ };
        cv::Mat out;
        cv::merge(v, out);
        return out;
    }
private:
    // Matrices for images
    cv::Mat blue_;
    cv::Mat red_;
    cv::Mat green_;

    cv::Mat blue_warped_;
    cv::Mat red_warped_;

    std::vector<cv::Mat> channels_{ 3 };

    // Matrices for descriptors
    cv::Mat descriptors1_;
    cv::Mat descriptors2_;
    cv::Mat descriptors3_;

    // Vectors for keypoints
    std::vector<cv::KeyPoint> kp1_;
    std::vector<cv::KeyPoint> kp2_;
    std::vector<cv::KeyPoint> kp3_;

    // Matrices for homography and mask
    cv::Mat homography_bg_;
    cv::Mat homography_rg_;
    cv::Mat mask_;

    // Vectors for homography
    std::vector<cv::Point2f> src_bg_pts_;
    std::vector<cv::Point2f> dst_bg_pts_;

    std::vector<cv::Point2f> src_rg_pts_;
    std::vector<cv::Point2f> dst_rg_pts_;

    // Vectors for matches and for good matches
    std::vector<std::vector<cv::DMatch>> matches_bg_;
    std::vector<cv::DMatch> good_matches_bg_;

    std::vector<std::vector<cv::DMatch>> matches_rg_;
    std::vector<cv::DMatch> good_matches_rg_;

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
        warpPerspectives();
    }

    void splitImage(const cv::Mat& img) {
        int width = img.cols;
        int height = img.rows;

        int channel_height = height / 3;
        int last_channel_height = height - 2 * channel_height;

        blue_ = channels_[0] = img(cv::Rect(0, 0, width, channel_height));
        green_ = channels_[1] = img(cv::Rect(0, channel_height, width, channel_height));
        red_ = channels_[2] = img(cv::Rect(0, 2 * channel_height, width, last_channel_height));
    }

    void findKeyPointsAndDescriptors() {
        cv::Ptr<cv::SIFT> sift{ cv::SIFT::create(max_features_) };
        sift->detectAndCompute(blue_, cv::Mat{}, kp1_, descriptors1_);
        sift->detectAndCompute(green_, cv::Mat{}, kp2_, descriptors2_);
        sift->detectAndCompute(red_, cv::Mat{}, kp3_, descriptors3_);
    }

    void matchFeatures() {
        descriptors1_.convertTo(descriptors1_, CV_32F);
        descriptors2_.convertTo(descriptors2_, CV_32F);
        descriptors3_.convertTo(descriptors3_, CV_32F);

        cv::FlannBasedMatcher macher{
          new cv::flann::KDTreeIndexParams(trees_number_),
          new cv::flann::SearchParams(number_of_checks_)
        };
        macher.knnMatch(descriptors1_, descriptors2_, matches_bg_, 2);
        macher.knnMatch(descriptors3_, descriptors2_, matches_rg_, 2);
    }

    void findGoodMatches() {
        good_matches_bg_ = matches_bg_ |
            std::views::filter([this](const auto& e) {return e[0].distance < lowe_ratio_ * e[1].distance; }) |
            std::views::transform([](const auto& e) {return e[0]; }) |
            std::ranges::to<std::vector>();

        good_matches_rg_ = matches_rg_ |
            std::views::filter([this](const auto& e) {return e[0].distance < lowe_ratio_ * e[1].distance; }) |
            std::views::transform([](const auto& e) {return e[0]; }) |
            std::ranges::to<std::vector>();

        std::ranges::sort(good_matches_bg_, std::less{});
        std::ranges::sort(good_matches_rg_, std::less{});
    }

    bool fillSrcDst() {
        if (good_matches_bg_.size() < min_match_count_) {
            return false;
        }

        if (good_matches_rg_.size() < min_match_count_) {
            return false;
        }

        src_bg_pts_.reserve(good_matches_bg_.size());
        dst_bg_pts_.reserve(good_matches_bg_.size());

        std::ranges::for_each(good_matches_bg_, [this](const auto& e) {
            src_bg_pts_.emplace_back(kp1_[e.queryIdx].pt);
            dst_bg_pts_.emplace_back(kp2_[e.trainIdx].pt);
            });

        src_rg_pts_.reserve(good_matches_rg_.size());
        dst_rg_pts_.reserve(good_matches_rg_.size());

        std::ranges::for_each(good_matches_rg_, [this](const auto& e) {
            src_rg_pts_.emplace_back(kp3_[e.queryIdx].pt);
            dst_rg_pts_.emplace_back(kp2_[e.trainIdx].pt);
            });

        return true;
    }

    void findHomography() {
        homography_bg_ = cv::findHomography(src_bg_pts_, dst_bg_pts_, cv::RANSAC, ransac_threshold_, mask_);
        homography_rg_ = cv::findHomography(src_rg_pts_, dst_rg_pts_, cv::RANSAC, ransac_threshold_, mask_);
    }

    void warpPerspectives() {
        cv::warpPerspective(blue_, blue_warped_, homography_bg_, green_.size());
        cv::warpPerspective(red_, red_warped_, homography_rg_, green_.size());
    }
};

int main() {
    std::filesystem::path path1{ "../data/images/emir.png" };
    if (!std::filesystem::exists(path1)) {
        std::cerr << std::format("Can't find file at given location: {}", path1.string());
        return EXIT_FAILURE;
    }

    FeatureMatching f{ path1 };
    auto img = f.warpedImages();

    cv::imshow("Warped", img);
    cv::waitKey(0);
    cv::destroyAllWindows();

    return 0;
}