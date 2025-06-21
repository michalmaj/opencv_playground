# Find Known Objects – ORB + FLANN + Homography

This demo locates a known object inside a scene image using **feature matching**  
combined with **homography estimation**. It is a classic pipeline used in:

- Augmented Reality (AR)
- Object detection in static scenes
- Visual inspection / quality control

---

## 📥 Input

- `data/images/book.png` – the object to find
- `data/images/book_scene.png` – the full image (scene) that contains the object

---

## 🧠 How It Works

### 1. ORB Keypoint Detection

```cpp
cv::ORB::create(max_features)->detectAndCompute(...)
```

### 2. Feature Matching with FLANN + Lowe's Ratio Test

- Descriptors are matched using cv::FlannBasedMatcher.
- Since ORB produces binary descriptors, they're converted to CV_32F.
```cpp
matcher.knnMatch(desc1, desc2, matches, 2);
```
- Lowe's ratio test filters good matches:
```cpp
match[0].distance < ratio * match[1].distance
```

### 3. Compute Homography with RANSAC

```cpp
cv::findHomography(src_pts, dst_pts, cv::RANSAC, ransac_thresh, mask)
```

- Filters out outliers.
- Ensures geometric consistency.

### 4. Project Object Corners

The object corners are transformed using the homography matrix
and projected onto the scene. Convex hull and red lines outline the detected object.

```cpp
cv::perspectiveTransform(...)
cv::convexHull(...)
cv::line(...)
```

---

## 🎛️ Parameters

| Parameter        | Description                               | Default |
| :--------------- | :---------------------------------------- | :------ |
| `max_features`   | ORB keypoint limit                        | 1000    |
| `min_match_count`| Minimum matches to attempt detection      | 10      |
| `lowe_ratio`     | Lowe's ratio for filtering matches        | 0.9     |
| `trees_number`   | KD-tree depth in FLANN                    | 5       |
| `number_of_checks`| FLANN search effort                      | 5       |
| `ransac_threshold`| Max reprojection error (px)              | 5.0     |

---

## 🖼️ Output

- Shows matched features and convex hull of detected object.
- Red outline shows estimated position in the scene.

---

## 🧪 Extensions

- Replace ORB with SIFT or SuperPoint for better accuracy.
- Use VideoCapture for real-time object tracking.
- Show rejected matches (outliers from RANSAC).

---

## 📌 Notes

- Works best if the object is flat and visible clearly in the scene.
- Sensitive to scale/rotation changes if keypoints are not invariant.