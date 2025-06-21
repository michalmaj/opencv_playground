# Image Inpainting Demo

This interactive demo demonstrates two classic image inpainting methods provided by OpenCV:

- **Navier-Stokes based method** (`cv::INPAINT_NS`)
- **Fast Marching (Telea) method** (`cv::INPAINT_TELEA`)

You can draw a mask over an image and then reconstruct the occluded regions using either method.

---

## 📥 Input

Required image path:

data/images/Lincoln.png

---

## 🧠 How Inpainting Works

Inpainting is a technique used to restore missing or damaged parts of an image using the surrounding context.  
In OpenCV, two algorithms are available via the `cv::inpaint()` function:

```cpp
cv::inpaint(input, mask, output, inpaintRadius, method);
```

Where:

- `input` – the damaged image,
- `mask` – a binary mask (`255` for damaged pixels),
- `inpaintRadius` – radius around each pixel used for reconstruction,
- `method` – algorithm used: `INPAINT_NS` or `INPAINT_TELEA`.

---

## 🔬 Method 1: Navier–Stokes Based Inpainting (cv::INPAINT_NS)

This method is inspired by fluid dynamics and is described in the paper:
"Navier–Stokes, Fluid Dynamics, and Image and Video Inpainting" – Bertalmío et al., 2001

🧪 **Key Ideas**

- Treat missing regions as a fluid that "flows in" from surrounding areas.
- Propagate isophotes (contours of equal intensity) into the region to be filled.
- Solve partial differential equations (PDEs) iteratively based on the Navier–Stokes model.

➕ **Pros**

- Preserves structure and directionality well (e.g., for thin lines, edges).
- Good for image restoration and old photo repair.

➖ **Cons**

- Computationally heavier.
- Sensitive to noise in the known region.
- May blur large textured areas.

---

## Method 2: Fast Marching (Telea) Method (cv::INPAINT_TELEA)

This is a faster, more heuristic method based on distance transforms and weighted averages.
Described in:

"An Image Inpainting Technique Based on the Fast Marching Method" – Alexandru Telea, 2004

🧪 **Key Ideas**

- Fill the region pixel by pixel, starting from the boundary of the mask.
- Use gradients and confidence weights from neighboring known pixels to infer missing values.
- Distance to the nearest known pixel influences the priority of filling.

➕ **Pros**

- Very fast (linear-time approximation).
- Good for small holes and simple occlusions.
- Easy to use interactively.

➖ **Cons**

- Less effective on complex structures (e.g., textures, edges).
- May cause oversmoothing in detailed areas.

---

## 🧰 Applications of Inpainting

- Photo/object removal
- Old photo or artwork restoration
- Removing scratches, logos, watermarks
- Video inpainting (with temporal consistency)
- Medical image reconstruction

---

## 🖱️ Interactivity

- Draw mask using the left mouse button (white lines over the image).
- Adjust line thickness and type via trackbars.
- Press:
    - `n` – run Navier-Stokes method
    - `t` – run Telea method
    - `r` – reset image and mask
    - `q` – quit

---

## 🎛️ Controls

### Trackbars:

- **Line Thickness:** (1–20)
- **Line Type:** (4-connected, 8-connected, anti-aliased)

### Mouse:

- Draw on image and mask using left-click.

---

## 🧠 Internals

```cpp
cv::inpaint(input_image, mask, output, inpaintRadius, method);
```

Where:

- mask defines damaged regions (white = damaged).
- inpaintRadius = 3
- method is either:
  - `cv::INPAINT_NS` – fluid-like Navier-Stokes model
  - `cv::INPAINT_TELEA` – fast marching approximation

---

## 📌 Notes

- The mask is resettable using key r.
- The result is shown in a separate window for each method.
- Good test for restoration, denoising, or artistic applications.

---

## 🔮 Ideas for Future

- Load predefined masks.
- Save output images to disk.
- Compare `PSNR` or `SSIM` between methods.
