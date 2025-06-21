# Align RGB Channels – Prokudin-Gorsky Image Alignment

This demo demonstrates how to **reconstruct a color image**  
from three vertically stacked grayscale images  
(e.g., from historical negatives or scanned color channels).

---

## 📥 Input

Provide a single vertically stacked grayscale image with channels ordered as:

```text
[Blue]
[Green]
[Red]
```


Example: `data/images/emir.png`

---

## ⚙️ Pipeline

1. **Split into Channels**

```cpp
int h = img.rows / 3;
blue = img(0:h)
green = img(h:2h)
red = img(2h:3h)
```

2. **Feature Matching Using `SIFT`**

- Detect keypoints in `blue`, `green`, and `red`
- Match `blue` ↔ `green` and `red` ↔ `green` using `FLANN`
- Filter with `Lowe’s ratio test`
- Compute homography using `RANSAC`

3. **Warp Perspective**

```cpp
cv::warpPerspective(blue, ..., H_blue_green);
cv::warpPerspective(red, ..., H_red_green);
```

- Align `blue` and `red` to match `green`

4. **Merge into RGB Image**

```cpp
cv::merge({ blue_aligned, green, red_aligned }, output);
```

---

## 🖼️ Output

Reconstructed full-color image with channels precisely aligned.

Useful for:

- Historical photography
- Digital restoration
- Computer vision education

---

## 🧠 Notes

- Relies on `SIFT` + `FLANN` + `RANSAC`
- Parameters can be tuned:
  - `max_features`, `lowe_ratio`, `ransac_threshold`
- Original image must have clearly defined details in each channel

