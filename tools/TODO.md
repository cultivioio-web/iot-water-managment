# 🛠️ Tools - TODO

**Status:** 🟡 In Progress  
**Priority:** Low  
**Purpose:** Development & Release Automation

---

## 📋 Tasks

### ✅ Completed
- [x] Release management scripts
- [x] Git flow setup scripts
- [x] Branch protection setup
- [x] PDF conversion scripts
- [x] S3 deployment script

### ⏳ Pending

#### CI/CD (High Priority)
- [ ] GitHub Actions workflow for firmware
  - [ ] Compile check on PR
  - [ ] Unit test runner
  - [ ] Release automation
- [ ] GitHub Actions for website
  - [ ] Deploy to S3/Netlify
  - [ ] Lighthouse audit
- [ ] Pre-commit hooks
  - [ ] Linting
  - [ ] Format check

#### Build Tools (Medium Priority)
- [ ] Firmware build script
- [ ] Multi-node flash script
- [ ] Version bump automation
- [ ] Changelog generator

#### Documentation (Low Priority)
- [ ] Tool usage guides
- [ ] CI/CD documentation
- [ ] Troubleshooting guide

---

## 📁 Current Tools

| Tool | Location | Purpose |
|------|----------|---------|
| Release Scripts | `tools/release-management/` | Git flow, releases |
| Conversion | `tools/scripts/` | PDF, deployment |

---

## 📝 Notes

- GitHub Actions should go in `.github/workflows/`
- See `tools/release-management/docs/` for guides

---

*Last Updated: December 3, 2025*

