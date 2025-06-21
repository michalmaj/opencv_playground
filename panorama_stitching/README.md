# Panorama Stitching – Automatic Image Mosaicing

This demo shows how to create a **panorama** from a series of overlapping images  
using OpenCV’s built-in `cv::Stitcher` class. It is useful for:

- Creating wide field-of-view scenes
- Reconstructing large structures from partial views
- Visual localization

---

## 📥 Input

Place multiple partially overlapping images in:

```text
data/images/scene/
├── image1.png
├── image2.png
├── image3.png
```

---

## ⚙️ How It Works

1. **Image Loading**
    - All `.jpg`, `.jpeg`, and `.png` files in the folder are loaded.
    - At least **2 images** are required.

```cpp
cv::imread(...)
std::filesystem::directory_iterator(...)
```

### 2. Stitcher Initialization

```cpp
stitcher_ = cv::Stitcher::create(mode);
```

- `cv::Stitcher::PANORAMA` (default) – general-purpose mode.
- `cv::Stitcher::SCANS` – for planar scenes (documents, etc).

### 3. Stitching

```cpp
auto status = stitcher_->stitch(images, output);
```

Internally uses:
- Feature detection (ORB/SIFT)
- Feature matching
- Camera estimation
- Bundle adjustment
- Seam estimation and blending

---

## ✅ Output

Displays the final stitched panoramic image in a resizable window.

---

## 🧪 Extensions

- Add saving the panorama to disk
- Measure stitching time and performance
- Show seam/blending visualization
- Use camera input to generate panoramas in real-time

---

## 📌 Notes

- Images must have overlap and consistent lighting
- Works best if taken from a single rotating viewpoint
- Fastest with resized input images (`cv::resize`) before stitching