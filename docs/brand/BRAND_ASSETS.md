# 🎨 Cultivio Brand Assets

## Logo Kit & Brand Guidelines

---

## 📁 Logo Files

### File Locations

```
Logo files are stored in TWO locations:

docs/assets/                    # Source/master files
└── cultivio-logo.svg           # Original logo

website/images/                 # Website-optimized files
├── cultivio-logo.svg           # Full logo (horizontal)
├── cultivio-logo-icon.svg      # Icon only (for app, favicon)
├── cultivio-logo-white.svg     # White version (dark backgrounds)
└── favicon.svg                 # Browser favicon (32x32 optimized)
```

---

## 🖼️ Logo Variants

### 1. Full Logo (Horizontal)
**File:** `cultivio-logo.svg`
**Use:** Website header, letterhead, presentations

```
┌─────────────────────────────────────────────────────────┐
│                                                         │
│     💧     C U L T I V I O                              │
│    🌿           SMART WATER MANAGEMENT                  │
│                                                         │
└─────────────────────────────────────────────────────────┘

Dimensions: 400 x 120 px (viewBox)
Minimum size: 150px width
```

### 2. Icon Only
**File:** `cultivio-logo-icon.svg`
**Use:** App icon, social media profile, small spaces

```
┌───────────┐
│           │
│    💧     │
│   🌿      │
│           │
└───────────┘

Dimensions: 100 x 100 px (viewBox)
Minimum size: 32px
```

### 3. White Logo
**File:** `cultivio-logo-white.svg`
**Use:** Dark backgrounds, footer, overlays

```
┌─────────────────────────────────────────────────────────┐
│ ▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓ │
│ ▓▓▓                                                 ▓▓▓ │
│ ▓▓▓     ○     C U L T I V I O                       ▓▓▓ │
│ ▓▓▓    ●           SMART WATER MANAGEMENT           ▓▓▓ │
│ ▓▓▓                                                 ▓▓▓ │
│ ▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓ │
└─────────────────────────────────────────────────────────┘
```

### 4. Favicon
**File:** `favicon.svg`
**Use:** Browser tab icon

```
┌─────┐
│ 💧  │  32 x 32 px optimized
└─────┘
```

---

## 🎨 Color Palette

### Primary Colors

| Color | Name | Hex | RGB | Use |
|-------|------|-----|-----|-----|
| ![#06b6d4](https://via.placeholder.com/20/06b6d4/06b6d4) | Cyan | `#06b6d4` | 6, 182, 212 | Primary brand |
| ![#3b82f6](https://via.placeholder.com/20/3b82f6/3b82f6) | Blue | `#3b82f6` | 59, 130, 246 | Gradient end |
| ![#10b981](https://via.placeholder.com/20/10b981/10b981) | Emerald | `#10b981` | 16, 185, 129 | Leaf/eco accent |

### Secondary Colors

| Color | Name | Hex | Use |
|-------|------|-----|-----|
| ![#1e3a5f](https://via.placeholder.com/20/1e3a5f/1e3a5f) | Deep Blue | `#1e3a5f` | Dark text, footer |
| ![#64748b](https://via.placeholder.com/20/64748b/64748b) | Slate | `#64748b` | Tagline, secondary text |
| ![#ffffff](https://via.placeholder.com/20/ffffff/ffffff) | White | `#ffffff` | Backgrounds |

### CSS Variables

```css
:root {
  --color-primary: #06b6d4;
  --color-primary-dark: #3b82f6;
  --color-accent: #10b981;
  --color-dark: #1e3a5f;
  --color-text: #64748b;
  --color-white: #ffffff;
  
  /* Gradients */
  --gradient-brand: linear-gradient(135deg, #06b6d4 0%, #3b82f6 100%);
  --gradient-leaf: linear-gradient(135deg, #10b981 0%, #06b6d4 100%);
}
```

---

## 📏 Logo Usage Guidelines

### ✅ DO

- Use on white or light gray backgrounds
- Maintain aspect ratio when scaling
- Use white version on dark backgrounds
- Keep clear space around logo (minimum = icon height)

### ❌ DON'T

- Stretch or distort the logo
- Change the colors
- Add effects (shadows, outlines)
- Place on busy/patterned backgrounds
- Use below minimum size (150px for full, 32px for icon)

---

## 📦 Logo for Different Uses

### Website

| Location | File | Notes |
|----------|------|-------|
| Header | `cultivio-logo.svg` | Link to homepage |
| Footer | `cultivio-logo-white.svg` | On dark background |
| Favicon | `favicon.svg` | Browser tab |
| App Page | `cultivio-logo-icon.svg` | Mobile optimized |

### Print / Packaging

| Use | File | Format Needed |
|-----|------|---------------|
| Product Box | `cultivio-logo.svg` | Convert to PDF/AI |
| Quick Start Guide | `cultivio-logo.svg` | 300 DPI PNG |
| Business Card | `cultivio-logo.svg` | Vector (AI/EPS) |
| Banner | `cultivio-logo.svg` | High-res PNG |

### Social Media

| Platform | Size | File |
|----------|------|------|
| Profile Picture | 400x400 | `cultivio-logo-icon.svg` → PNG |
| Facebook Cover | 820x312 | `cultivio-logo.svg` centered |
| Twitter Header | 1500x500 | `cultivio-logo.svg` |
| Instagram Post | 1080x1080 | `cultivio-logo.svg` centered |

---

## 🔧 How to Export

### SVG to PNG (High Resolution)

Using **Inkscape** (free):
```
1. Open SVG file
2. File → Export PNG Image
3. Set DPI to 300 (for print)
4. Export
```

Using **Online Tool**:
```
1. Go to svgtopng.com
2. Upload SVG
3. Set width (e.g., 2000px for print)
4. Download PNG
```

### SVG to PDF (for Print)

Using **Adobe Illustrator**:
```
1. Open SVG
2. File → Save As → PDF
3. Select "Press Quality"
4. Save
```

Using **Inkscape**:
```
1. Open SVG
2. File → Save As → PDF
3. Use default settings
4. Save
```

---

## 📱 App Icons

### PWA Icons (for manifest.json)

Generate these sizes from `cultivio-logo-icon.svg`:

| Size | Use |
|------|-----|
| 72x72 | Android (ldpi) |
| 96x96 | Android (mdpi) |
| 128x128 | Chrome Web Store |
| 144x144 | Android (xhdpi) |
| 152x152 | iPad |
| 192x192 | Android (xxhdpi) |
| 384x384 | Android (xxxhdpi) |
| 512x512 | Google Play Store |

### Generate with CLI

```bash
# Using ImageMagick
convert -background none -resize 192x192 cultivio-logo-icon.svg icon-192.png
convert -background none -resize 512x512 cultivio-logo-icon.svg icon-512.png
```

---

## 🏷️ Product Labels

### For Device Enclosures

Small label (20x10mm):
```
┌────────────────────┐
│ 💧 CULTIVIO        │
│    AquaSense       │
└────────────────────┘
```

### For Retail Box

Full logo with tagline (as in logo file)

---

## 📞 Brand Contact

For logo files in other formats or usage questions:

**Email:** brand@cultivio.io
**Website:** cultivio.io/brand

---

*© 2025 Cultivio Technologies. All rights reserved.*
*Logo and brand assets are trademarks of Cultivio Technologies.*

