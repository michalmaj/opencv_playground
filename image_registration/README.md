# Image Registration – ORB + Homography

This demo performs automatic image registration using **keypoint matching and homography estimation**.  
It aligns one image (e.g. a scanned form) with another (e.g. template form) using ORB features and RANSAC.

---

## 📥 Input

- `data/images/scanned-form.png`
- `data/images/form.png`

---

## 🧠 How It Works

### Step 1: Detect ORB keypoints and compute descriptors

```cpp
cv::ORB::create(num_features)->detectAndCompute(...)
```

### Step 2: Match descriptors

```cpp
cv::DescriptorMatcher::create("BruteForce-Hamming")->match(...)
```

- All descriptors are matched, then sorted by distance.
- The top `N%` best matches are selected.

### Step 3: Estimate homography with RANSAC

```cpp
cv::findHomography(src_pts, dst_pts, cv::RANSAC, 3.0, mask)
```

- Robust estimation rejects outliers using RANSAC.
- Only inliers are used for warping.

### Step 4: Apply perspective warp

```cpp
cv::warpPerspective(src, warped, H, dst.size());
```

- The source image is projected into the plane of the destination.

---

## 🖼️ Output

- `Matched` – shows feature matches (inliers with optional mask).
- `Perspective` – warped version of the source image.

---

## 🎛️ Parameters

- num_features = 500: maximum number of ORB keypoints
- percent_features = 15%: how many top matches to use
- matcher = "BruteForce-Hamming": default matcher for ORB

---

## 📌 Notes

- Works well for planar documents and printed media.
- The result can be refined using optical flow or fine-tuned descriptors.
- For better accuracy, consider using SuperPoint or SIFT+FLANN.

---

## 🧪 Extensions

- Visualize inliers vs outliers (RANSAC mask).
- Allow fine-tuning match threshold.
- Save aligned output to disk.
- Batch process scanned documents for OCR preprocessing.