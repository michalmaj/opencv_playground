# Coin Detection – Thresholding and Morphology

This interactive demo allows real-time image processing for detecting objects (e.g., coins)  
using thresholding and morphological operations.

---

## 📥 Input

Required image path:

data/images/CoinsA.png


---

## 🎛️ Interactive Controls

### Threshold Controls (`Threshold Controls` window)

- **Threshold Types:** choose between binary, binary inverted, trunc, etc.
- **Min Value / Max Value:** control the clipping threshold

### Morphology Controls (`Morphology Controls` window)

- **Morph Type:** erosion, dilation, opening, closing
- **Kernel Shape:** rectangular, cross, elliptical
- **Kernel Size Multiplier:** size of the structuring element
- **Number of Iterations:** how many times to apply the operation

---

## 🧠 Internals

### Thresholding

```cpp
cv::threshold(gray, thresholded, min, max, type);
```

---

## Morphological operations

```cpp
cv::erode / cv::dilate / cv::morphologyEx
```

The kernel is computed using `cv::getStructuringElement`.

---

## ⌨️ Hotkeys

- q — quit
- c — reset to grayscale
- b/g/r — show blue, green, red channel respectively

---

## 📌 Notes

- You can combine thresholding and morphology to clean noise and prepare for contour detection.
- This is a good base for blob/contour-based object detection or coin counting.

---

## 🧪 Extensions (Future Ideas)

- Add cv::findContours to highlight each coin.
- Use connected components or Hough Circles for automatic counting.
- Display histogram of each channel.