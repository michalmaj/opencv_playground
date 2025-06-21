# Homography Warp – Manual Perspective Mapping

This demo allows interactive computation of a **homography matrix** from 4 manually selected points  
and applies a perspective warp to project one image into the plane of another.

Useful for:
- Rectifying planar objects (e.g. documents, billboards),
- Simulating AR projections,
- Image compositing based on perspective.

---

## 📥 Input

- Source image: `data/images/first-image.png`
- Destination image: `data/images/times-square.png`

---

## 🎛️ Controls

- **Left click** on:
    - `Src` window → to set **source points** (max 4)
    - `Dst` window → to set **destination points** (max 4)

### ⌨️ Hotkeys

- `p` – compute and display warped image
- `f` – full projection into destination image
- `r` – reset all points
- `q` – quit

---

## 🧠 How It Works

### 1. Select 4 points on both images

These define a mapping between two planes.

### 2. Compute homography matrix

Using:

```cpp
cv::Mat H = cv::findHomography(srcPoints, dstPoints);
```

### 3. Warp the source image

Apply the projective transformation:

```cpp
cv::warpPerspective(src, warped, H, dst_size);
```

Optionally blend the warped image into the destination using masks.

---

## 📌 Notes

- Points are sorted to maintain consistent ordering (top-left → top-right → bottom-right → bottom-left).
- If no destination image is provided, a bounding box is inferred to warp the object independently.
- This technique works only for planar surfaces.

---

## 🧪 Extensions

- Add saving of warped result.
- Visualize selected points with markers.
- Allow reordering or dragging of points.
- Use SIFT/SURF/ORB features for automatic point detection.