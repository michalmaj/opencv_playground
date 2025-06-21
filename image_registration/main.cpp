//
// Created by Michał Maj on 21/06/2025.
//

#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/photo.hpp>
#include <filesystem>
#include <ranges>

class ImageRegistration {
public:
    ImageRegistration(const std::filesystem::path& src_path,
        const std::filesystem::path& dst_path,
        int num_features = 500,
        float percent_features = 0.15f,
        std::string&& matcher_type = "BruteForce-Hamming"
    ) :
        src_(cv::imread(src_path.string())),
        dst_(cv::imread(dst_path.string())),
        num_features_(num_features),
        percent_features_(percent_features),
        matcher_type_(std::move(matcher_type)) {
        if (src_.empty()) {
            throw std::runtime_error(std::format("Can't load an image from: {}", src_path.string()));
        }

        if (dst_.empty()) {
            throw std::runtime_error(std::format("Can't load an image from: {}", dst_path.string()));
        }

        process();
    }

    const auto& getSrc() const { return src_; }
    const auto& getDst() const { return dst_; }
    const auto& getWarped() const { return warped_; }

    cv::Mat showMatches(bool use_ransac_mask = true) {
        cv::Mat mask{};
        if (use_ransac_mask) {
            mask = mask_;
        }
        cv::Mat output;
        cv::drawMatches(src_,
            kp1_,
            dst_,
            kp2_,
            matches_,
            output,
            cv::Scalar::all(-1),
            cv::Scalar::all(-1),
            mask);

        return output;
    }
private:
    // Matrices for images
    cv::Mat src_;
    cv::Mat dst_;
    cv::Mat gray_;

    // Matrices for descriptors
    cv::Mat descriptors1_;
    cv::Mat descriptors2_;

    // Matrix for mask
    cv::Mat mask_;

    // Matrix for homography
    cv::Mat homography_;

    // Matrix for warped image
    cv::Mat warped_;

    // Store keypoints
    std::vector<cv::KeyPoint> kp1_;
    std::vector<cv::KeyPoint> kp2_;

    // Store matches
    std::vector<cv::DMatch> matches_;

    // Store points for homography
    std::vector<cv::Point> src_pts_;
    std::vector<cv::Point> dst_pts_;

    // Values for setup detect and match keypoints
    int num_features_{};
    float percent_features_{};
    std::string matcher_type_{};

    void process() {
        findKeyPointsAndDescriptors();
        matchFeatures();
        getGoodMatches();
        auto done = calculateHomography();
        if (!done) {
            std::cerr << "No enough points to calculate homography\n";
            return;
        }
        warpImage();
    }

    void convertToGray(const cv::Mat& img) {
        cv::cvtColor(img, gray_, cv::COLOR_BGR2GRAY);
    }

    void findKeyPointsAndDescriptors() {
        cv::Ptr<cv::ORB> orb{ cv::ORB::create(num_features_) };
        convertToGray(src_);
        orb->detectAndCompute(gray_, cv::Mat{}, kp1_, descriptors1_);
        convertToGray(dst_);
        orb->detectAndCompute(gray_, cv::Mat{}, kp2_, descriptors2_);
    }

    void matchFeatures() {
        cv::Ptr<cv::DescriptorMatcher> matcher{ cv::DescriptorMatcher::create(matcher_type_) };
        matcher->match(descriptors1_, descriptors2_, matches_);
        std::ranges::sort(matches_, std::less{});
    }

    void getGoodMatches() {
        int take{ static_cast<int>(matches_.size() * percent_features_) };
        matches_ = matches_ | std::views::take(take) | std::ranges::to<std::vector>();
    }

    bool calculateHomography() {
        if (matches_.size() < 4) {
            return false;
        }

        for (const auto& match : matches_) {
            src_pts_.emplace_back(kp1_[match.queryIdx].pt);
            dst_pts_.emplace_back(kp2_[match.trainIdx].pt);
        }

        homography_ = cv::findHomography(src_pts_, dst_pts_, cv::RANSAC, 3.0, mask_);
        std::cout << homography_ << '\n';
        return true;
    }

    void warpImage() {
        cv::warpPerspective(src_, warped_, homography_, dst_.size());
    }
};

int main() {
    std::filesystem::path path1{ "../data/images/scanned-form.png" };
    if (!std::filesystem::exists(path1)) {
        std::cerr << std::format("Can't find file at given location: {}", path1.string());
        return EXIT_FAILURE;
    }

    std::filesystem::path path2{ "../data/images/form.png" };
    if (!std::filesystem::exists(path2)) {
        std::cerr << std::format("Can't find file at given location: {}", path2.string());
        return EXIT_FAILURE;
    }

    ImageRegistration ir(path1, path2);

    auto matched = ir.showMatches();

    cv::imshow("Matched", matched);
    cv::imshow("Perspective", ir.getWarped());
    cv::waitKey(0);
    cv::destroyAllWindows();


    return 0;
}