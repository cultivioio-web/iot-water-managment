# 📄 Converting Business Documents to PDF

This guide explains how to convert the Cultivio business documentation from Markdown (.md) to PDF format.

---

## 🚀 **QUICK START**

### **Windows (PowerShell)**
```powershell
cd scripts
.\convert-business-docs-to-pdf.ps1
```

### **Windows (Batch)**
```cmd
cd scripts
convert-to-pdf.bat
```

---

## 📋 **AVAILABLE FILES**

The following business documents will be converted:

1. ✅ **FINANCIAL_MODEL.md** - Financial projections and analysis
2. ✅ **INVESTOR_ONEPAGER.md** - One-page investor summary
3. ✅ **INVESTOR_PITCH.md** - Full investor pitch deck
4. ✅ **INVESTOR_ROADMAP.md** - Product and business roadmap
5. ✅ **PRODUCTION_COST_ANALYSIS.md** - Manufacturing cost breakdown
6. ✅ **SUBSCRIPTION_TODO.md** - Subscription model planning

---

## 🛠️ **INSTALLATION OPTIONS**

### **Option 1: Pandoc (Recommended) ⭐**

**What is Pandoc?**
Universal document converter that produces high-quality PDFs.

**Installation:**

#### Windows (Chocolatey):
```powershell
choco install pandoc miktex -y
```

#### Windows (Manual):
1. Download Pandoc: https://pandoc.org/installing.html
2. Download MiKTeX: https://miktex.org/download
3. Install both applications
4. Restart terminal

#### Verify Installation:
```bash
pandoc --version
```

---

### **Option 2: NPM markdown-pdf**

**Installation:**
```bash
npm install -g markdown-pdf
```

**Usage:**
```bash
cd docs/business
markdown-pdf FINANCIAL_MODEL.md -o pdf/FINANCIAL_MODEL.pdf
```

---

### **Option 3: VS Code Extension** 📝

**Steps:**
1. Open VS Code
2. Install extension: "Markdown PDF" by yzane
3. Open any .md file
4. Press `Ctrl+Shift+P`
5. Type: "Markdown PDF: Export (pdf)"
6. Select location to save

**Extension Link:**
https://marketplace.visualstudio.com/items?itemName=yzane.markdown-pdf

---

### **Option 4: Online Converter** 🌐

**Free Online Tools:**

1. **MD2PDF** (Best for business docs)
   - URL: https://md2pdf.netlify.app/
   - Supports styling
   - No registration required

2. **Markdown to PDF**
   - URL: https://www.markdowntopdf.com/
   - Clean output
   - Preserves formatting

3. **Dillinger**
   - URL: https://dillinger.io/
   - Online editor + export
   - Multiple formats

**Steps:**
1. Visit website
2. Upload or paste markdown content
3. Click "Export to PDF"
4. Download the PDF

---

## 🎨 **CUSTOMIZATION OPTIONS**

### **Pandoc Advanced Options**

#### Professional Style:
```bash
pandoc FINANCIAL_MODEL.md -o FINANCIAL_MODEL.pdf \
  --pdf-engine=xelatex \
  -V geometry:margin=1in \
  -V fontsize=11pt \
  -V documentclass=article \
  --toc \
  --toc-depth=3 \
  -s
```

#### With Custom CSS:
```bash
pandoc INVESTOR_PITCH.md -o INVESTOR_PITCH.pdf \
  --pdf-engine=wkhtmltopdf \
  --css=style.css
```

#### Landscape Orientation:
```bash
pandoc document.md -o document.pdf \
  -V geometry:landscape \
  -V geometry:margin=0.5in
```

---

## 📂 **OUTPUT LOCATION**

PDFs will be saved to:
```
docs/business/pdf/
├── FINANCIAL_MODEL.pdf
├── INVESTOR_ONEPAGER.pdf
├── INVESTOR_PITCH.pdf
├── INVESTOR_ROADMAP.pdf
├── PRODUCTION_COST_ANALYSIS.pdf
└── SUBSCRIPTION_TODO.pdf
```

---

## 🔧 **TROUBLESHOOTING**

### **"pandoc: command not found"**
- **Solution:** Pandoc not in PATH. Restart terminal or add to PATH manually.

### **"! LaTeX Error: File not found"**
- **Solution:** Install MiKTeX (LaTeX distribution)
  ```bash
  choco install miktex -y
  ```

### **"PDF engine not found"**
- **Solution:** Specify different engine:
  ```bash
  pandoc file.md -o file.pdf --pdf-engine=wkhtmltopdf
  ```

### **"Permission denied"**
- **Solution:** Run PowerShell as Administrator:
  ```powershell
  Set-ExecutionPolicy -Scope CurrentUser -ExecutionPolicy RemoteSigned
  ```

### **Poor PDF Quality**
- **Solution:** Use XeLaTeX engine:
  ```bash
  pandoc file.md -o file.pdf --pdf-engine=xelatex
  ```

---

## 💡 **TIPS & BEST PRACTICES**

### **For Investor Documents:**
1. ✅ Use professional templates
2. ✅ Include table of contents
3. ✅ Add page numbers
4. ✅ Use consistent formatting
5. ✅ Compress large PDFs

### **Quality Settings:**
```bash
# High quality (larger file size)
pandoc -o output.pdf --dpi=300

# Compressed (smaller file size)
pandoc -o output.pdf --dpi=150
```

### **Batch Conversion:**
```powershell
# Convert all at once
Get-ChildItem *.md | ForEach-Object {
    pandoc $_.FullName -o "pdf\$($_.BaseName).pdf"
}
```

---

## 📊 **ALTERNATIVE FORMATS**

### **Convert to DOCX:**
```bash
pandoc FINANCIAL_MODEL.md -o FINANCIAL_MODEL.docx
```

### **Convert to HTML:**
```bash
pandoc INVESTOR_PITCH.md -o INVESTOR_PITCH.html --standalone
```

### **Convert to PowerPoint:**
```bash
pandoc INVESTOR_PITCH.md -o INVESTOR_PITCH.pptx
```

---

## 🎯 **QUICK REFERENCE**

| Command | Description |
|---------|-------------|
| `pandoc file.md -o file.pdf` | Basic conversion |
| `pandoc file.md -o file.pdf --toc` | With table of contents |
| `pandoc file.md -o file.pdf -s` | Standalone document |
| `pandoc *.md -o combined.pdf` | Combine multiple files |
| `pandoc file.md -o file.pdf --pdf-engine=xelatex` | Use XeLaTeX engine |

---

## 📞 **NEED HELP?**

### **Resources:**
- Pandoc Manual: https://pandoc.org/MANUAL.html
- Pandoc Demos: https://pandoc.org/demos.html
- Stack Overflow: https://stackoverflow.com/questions/tagged/pandoc

### **Contact:**
- Email: hello@cultivio.in
- Internal: Check with tech team

---

## ✅ **CHECKLIST**

Before sharing with investors:

- [ ] All PDFs generated successfully
- [ ] Table of contents included
- [ ] Page numbers present
- [ ] Links are working (if any)
- [ ] Images display correctly
- [ ] Formatting is consistent
- [ ] File size is reasonable (<5MB each)
- [ ] PDFs are searchable
- [ ] Metadata is correct
- [ ] File names are professional

---

## 📝 **VERSION CONTROL**

When updating documents:

1. Update the .md source file
2. Regenerate PDF
3. Check for formatting issues
4. Compare with previous version
5. Update version number/date
6. Archive old versions

---

**Last Updated:** December 3, 2025
**Maintained By:** Cultivio Tech Team

