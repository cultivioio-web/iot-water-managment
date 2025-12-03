# 🖼️ Cultivio Website Images

This folder contains all image assets for the Cultivio website, organized by category.

---

## 📁 Folder Structure

```
website/images/
├── README.md                    # This file
│
├── cultivio-logo.svg            # Full horizontal logo
├── cultivio-logo-icon.svg       # Icon only (square)
├── cultivio-logo-white.svg      # White version for dark backgrounds
├── favicon.svg                  # Browser favicon (32x32 optimized)
│
├── hero-banner.svg              # Hero section banner
├── phone-mockup.svg             # Phone with app mockup
│
├── products/                    # Product illustrations
│   ├── sensor-node.svg          # Sensor node device
│   ├── controller-node.svg      # Controller node device
│   ├── router-node.svg          # Router node device
│   ├── ultrasonic-sensor.svg    # JSN-SR04T sensor
│   └── complete-system.svg      # Full system diagram
│
├── icons/                       # Feature icons
│   ├── auto-pump.svg            # Automatic pump control
│   ├── no-wifi.svg              # No WiFi required
│   ├── mobile-app.svg           # Mobile app control
│   ├── water-save.svg           # Water saving
│   ├── wireless-range.svg       # Wireless range
│   └── warranty.svg             # Warranty shield
│
├── how-it-works/                # Step-by-step illustrations
│   ├── step1-install.svg        # Install sensor
│   ├── step2-connect.svg        # Connect controller
│   ├── step3-setup.svg          # Setup via app
│   └── step4-enjoy.svg          # Enjoy automation
│
└── packaging/                   # Packaging artwork
    ├── box-front.svg            # Box front face design
    └── basic-kit.svg            # Kit contents illustration
```

---

## 🎨 Image Usage Guide

### Logo Files

| File | Dimensions | Use Case |
|------|------------|----------|
| `cultivio-logo.svg` | 400×120 | Website header, footer, marketing |
| `cultivio-logo-icon.svg` | 100×100 | App icon, social media profile |
| `cultivio-logo-white.svg` | 400×120 | Dark backgrounds |
| `favicon.svg` | 32×32 | Browser tab icon |

### Product Images

| File | Description |
|------|-------------|
| `products/sensor-node.svg` | Sensor node illustration |
| `products/controller-node.svg` | Controller with relay |
| `products/router-node.svg` | Range extender |
| `products/ultrasonic-sensor.svg` | JSN-SR04T waterproof sensor |
| `products/complete-system.svg` | Full system with all components |

### Feature Icons (64×64)

Use these for feature highlights on the website:
- `icons/auto-pump.svg` - Automatic pump control
- `icons/no-wifi.svg` - No internet required
- `icons/mobile-app.svg` - Mobile app control
- `icons/water-save.svg` - Water saving
- `icons/wireless-range.svg` - 30m wireless range
- `icons/warranty.svg` - 6 month warranty

### How It Works (200×200)

Step-by-step illustrations:
1. `how-it-works/step1-install.svg` - Mount sensor in tank
2. `how-it-works/step2-connect.svg` - Wire controller to pump
3. `how-it-works/step3-setup.svg` - Configure via mobile app
4. `how-it-works/step4-enjoy.svg` - Automatic operation

---

## 🔧 How to Use in HTML

### Logo in Header
```html
<img src="images/cultivio-logo.svg" alt="Cultivio" width="150">
```

### Feature Icon
```html
<img src="images/icons/auto-pump.svg" alt="Auto Pump" width="64" height="64">
```

### Product Image
```html
<img src="images/products/complete-system.svg" alt="AquaSense System" width="600">
```

### Background in CSS
```css
.hero {
  background-image: url('images/hero-banner.svg');
  background-size: cover;
}
```

---

## 📐 Design Specifications

### Color Palette Used

| Color | Hex | CSS Variable |
|-------|-----|--------------|
| Primary Cyan | `#06b6d4` | `var(--color-primary)` |
| Primary Blue | `#3b82f6` | `var(--color-primary-dark)` |
| Accent Green | `#10b981` | `var(--color-accent)` |
| Dark | `#1e293b` | `var(--color-dark)` |
| Text Gray | `#64748b` | `var(--color-text)` |

### Gradients

```css
/* Brand Gradient */
background: linear-gradient(135deg, #06b6d4 0%, #3b82f6 100%);

/* Leaf/Eco Gradient */
background: linear-gradient(135deg, #10b981 0%, #06b6d4 100%);
```

---

## 🖨️ For Print/Packaging

For print-ready files, use:
- `../hardware/packaging/assets/cultivio-logo-print.svg`

See `docs/brand/BRAND_ASSETS.md` for:
- CMYK color values
- Minimum print sizes
- Export instructions

---

## 📱 Generating PNG Versions

To convert SVG to PNG for social media:

### Using Inkscape (Free)
```bash
inkscape --export-type=png --export-width=1200 hero-banner.svg
```

### Using ImageMagick
```bash
convert -density 300 -background none logo.svg logo.png
```

### Online Tools
- [SVG to PNG](https://svgtopng.com)
- [CloudConvert](https://cloudconvert.com)

---

## ✅ Checklist for Website

- [x] Logo files
- [x] Favicon
- [x] Hero banner
- [x] Phone mockup
- [x] Product illustrations (3 devices)
- [x] Ultrasonic sensor
- [x] Complete system diagram
- [x] Feature icons (6)
- [x] How it works steps (4)
- [x] Packaging artwork

---

*All images are SVG format for scalability and small file size.*
*© 2025 Cultivio Technologies. All rights reserved.*

