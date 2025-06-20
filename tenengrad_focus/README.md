# Tenengrad Focus Measure Demo

This demo evaluates the sharpness of video frames using the **Tenengrad focus measure**,  
based on gradient magnitude computed with the Sobel operator.

It processes a video frame-by-frame, extracts a center ROI (Region of Interest),  
converts it to grayscale, and calculates a sharpness score. The frame with the highest score  
is shown as the best-focused frame.

---

## 📚 Method

The Tenengrad focus measure is based on the **sum of squared gradient magnitudes** computed with the Sobel operator:

$$
T = \sum_{x,y} \left( G_x(x, y)^2 + G_y(x, y)^2 \right)
$$

Where:

- `Gx(x, y)` is the Sobel gradient in the **horizontal direction**
- `Gy(x, y)` is the Sobel gradient in the **vertical direction**

The sharper the image, the higher the value of `T`, since sharp images contain more high-frequency components (strong edges).

**Reference:**  
[Diatom Autofocusing in Brightfield Microscopy – A Comparative Study (2006)](https://www.researchgate.net/publication/3887632_Diatom_autofocusing_in_brightfield_microscopy_A_comparative_study)

---

## 🔧 Parameters

- **ROI size:** 10% of frame dimensions around the center
- **Grayscale conversion:** BGR → Grayscale
- **Sobel kernel size:** default (3x3)

---

## ▶️ Running the Demo

Make sure you have a test video at:
data/videos/focus-test.mp4

Then from the `build/` directory:

```bash
./tenengrad_focus
```

You will see:

- Live preview of frames from the video
- Extracted ROI
- The sharpest frame at the end of processing

---

## 📷 Example use cases

- Autofocus systems (microscopy, robotics, webcams)
- Image quality assessment
- Camera calibration

---

## 📝 Notes

- The function *calculateTenengradFocus()* expects a grayscale image.
- The code uses *cv::Sobel()* and computes the magnitude using element-wise operations.
