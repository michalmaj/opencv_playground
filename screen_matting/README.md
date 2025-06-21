# Screen Matting (Greenscreen Background Replacement)

This demo allows you to interactively select a background color from a video frame  
and replace it with a static background image.

The method is based on simple **color segmentation in CIE Lab space**, refined by  
blur and erosion to soften the edges.

---

## 📥 Input

- Video file: `data/videos/greenscreen-asteroid.mp4`
- Background image: `data/images/IF4.png`

---

## 🎛️ Controls

- **Left-click** on the video to pick background colors.
- **Trackbars:**
    - `Blur`: softens the mask edges.
    - `Erode`: reduces noise in the mask.

### ⌨️ Hotkeys

- `q` — quit

---

## 🧠 How it works

### Step 1: Convert to Lab color space

The Lab color space separates luminance (L) from chromaticity (a, b),  
making it more robust for color-based segmentation.

### Step 2: Select background color

You can click on the video multiple times to select several points.  
A bounding range is computed from all picked colors.

```cpp
cv::inRange(lab_img, lower, upper, mask);
```

### Step 3: Mask refinement

- The binary mask is softened using `cv::blur()`.
- Noise is reduced using `cv::erode()` with a cross-shaped kernel.

### Step 4: Compositing

- The mask is inverted (`bitwise_not`).
- The new background is resized to the video frame size.
- The masked regions of the original and background are combined:
```cpp 
bitwise_and(background, ~mask, temp)
bitwise_and(frame, mask, frame)
bitwise_or(frame, temp, result)
```

---

## 📌 Notes

- Works best with uniform background colors (e.g., green screen).
- A simple alternative to chroma keying or deep matting.
- You can replace `cv::inRange` with a trained color classifier for better robustness.

---

## 🧪 Extensions

- Add `medianBlur()` for better edge handling.
- Integrate semantic segmentation for more intelligent masking.
- Export processed video using `cv::VideoWriter`.

