# 🔗 Linear Integration Guide

This document explains how to use Linear for project management with our GitLab CI/CD workflow.

## Overview

```
┌─────────────┐      ┌─────────────┐      ┌─────────────┐
│   LINEAR    │ ←──→ │   GITLAB    │ ←──→ │   CI/CD     │
│   Issues    │      │   Commits   │      │   Pipeline  │
└─────────────┘      └─────────────┘      └─────────────┘
```

## 🎯 Workflow

### 1. Create Issue in Linear

Create your task/feature/bug in Linear. Each issue gets a unique ID like:
- `CUL-123` (Cultivio project)
- `AQS-45` (AquaSense sub-project)

### 2. Branch Naming Convention

**Always include the Linear issue ID in your branch name:**

```bash
# Format: type/ISSUE-ID-short-description

# Features
git checkout -b feat/CUL-123-add-water-level-alerts

# Bug fixes
git checkout -b fix/CUL-124-pump-timeout-error

# Improvements
git checkout -b improve/CUL-125-optimize-zigbee-range

# Documentation
git checkout -b docs/CUL-126-update-assembly-guide

# Chores (maintenance)
git checkout -b chore/CUL-127-update-dependencies
```

### 3. Commit Message Format

Reference Linear issues in commits for automatic linking:

```bash
# Simple reference
git commit -m "Add water level alerts CUL-123"

# With action keywords (auto-updates issue status)
git commit -m "feat: Add push notifications

Implements water level threshold alerts with configurable
high/low warnings.

Closes CUL-123"
```

**Action Keywords:**
| Keyword | Linear Status |
|---------|---------------|
| `Closes CUL-123` | → Done |
| `Fixes CUL-123` | → Done |
| `Resolves CUL-123` | → Done |
| `Refs CUL-123` | → Links only |

### 4. Merge Request Template

When creating a MR in GitLab, use this template:

```markdown
## Summary
Brief description of changes

## Linear Issue
CUL-123

## Type of Change
- [ ] 🐛 Bug fix
- [ ] ✨ New feature
- [ ] 📚 Documentation
- [ ] 🔧 Maintenance

## Checklist
- [ ] Code follows project style
- [ ] Tests pass locally
- [ ] Documentation updated
- [ ] Linear issue linked
```

---

## ⚙️ Setting Up Linear-GitLab Integration

### Option 1: Linear's Native GitLab Integration

1. Go to **Linear Settings** → **Integrations** → **GitLab**
2. Click **Connect GitLab**
3. Authorize Linear to access your GitLab
4. Select your repository: `cultivio/iot-water-management`
5. Enable:
   - ✅ Auto-link commits mentioning issue IDs
   - ✅ Auto-close issues on merge
   - ✅ Show MR status on issues

### Option 2: Webhook Integration (Manual)

If native integration isn't available:

1. In **GitLab** → **Settings** → **Webhooks**
2. Add webhook URL: `https://api.linear.app/webhooks/gitlab`
3. Select triggers:
   - ✅ Push events
   - ✅ Merge request events
   - ✅ Pipeline events

---

## 📋 Linear Project Structure

Recommended Linear setup for Cultivio:

```
Cultivio (Team)
├── 🎯 Backlog
│   ├── CUL-xxx - Features to implement
│   └── CUL-xxx - Bugs to fix
│
├── 🚀 Sprint (Current)
│   ├── CUL-xxx - In Progress
│   └── CUL-xxx - In Review
│
├── ✅ Done
│   └── CUL-xxx - Completed
│
└── 🏷️ Labels
    ├── firmware
    ├── hardware
    ├── website
    ├── mobile-app
    ├── documentation
    └── urgent
```

### Recommended Labels

| Label | Color | Use For |
|-------|-------|---------|
| `firmware` | 🔵 Blue | ESP32 code changes |
| `hardware` | 🟠 Orange | PCB, enclosure, BOM |
| `website` | 🟢 Green | Website updates |
| `mobile-app` | 🟣 Purple | Provisioning app |
| `documentation` | ⚪ Gray | Docs and guides |
| `urgent` | 🔴 Red | Critical issues |
| `blocked` | 🟡 Yellow | Waiting on something |

---

## 🔄 Complete Workflow Example

### Scenario: Add Low Battery Warning Feature

**Step 1: Create Linear Issue**
```
Title: Add low battery warning for sensor nodes
ID: CUL-234
Label: firmware
Priority: Medium
```

**Step 2: Create Branch**
```bash
git checkout develop
git pull origin develop
git checkout -b feat/CUL-234-low-battery-warning
```

**Step 3: Make Changes & Commit**
```bash
# Make your code changes...

git add .
git commit -m "feat: Add low battery warning system

- Add battery voltage monitoring
- Implement BLE notification for low battery
- Add LED indicator pattern for low battery

Refs CUL-234"
```

**Step 4: Push & Create MR**
```bash
git push -u origin feat/CUL-234-low-battery-warning
```

Then create MR in GitLab with title:
```
feat: Add low battery warning [CUL-234]
```

**Step 5: After Merge**
- Linear issue auto-moves to "Done"
- GitLab shows linked issue
- CI/CD pipeline runs tests & builds

---

## 📊 GitLab CI + Linear Status

The pipeline status can be shown in Linear:

```yaml
# In .gitlab-ci.yml (already configured)
# When pipeline passes/fails, Linear shows the status
```

**Linear Issue View:**
```
┌────────────────────────────────────────┐
│ CUL-234: Add low battery warning       │
├────────────────────────────────────────┤
│ Status: In Review                      │
│ Branch: feat/CUL-234-low-battery       │
│ Pipeline: ✅ Passed                    │
│ MR: !45 (Ready to merge)               │
└────────────────────────────────────────┘
```

---

## 🚀 Quick Reference

### Branch Types
| Prefix | Use Case |
|--------|----------|
| `feat/` | New features |
| `fix/` | Bug fixes |
| `improve/` | Enhancements |
| `docs/` | Documentation |
| `chore/` | Maintenance |
| `test/` | Test additions |
| `refactor/` | Code refactoring |

### Commit Prefixes (Conventional Commits)
| Prefix | Description |
|--------|-------------|
| `feat:` | New feature |
| `fix:` | Bug fix |
| `docs:` | Documentation |
| `style:` | Code style (formatting) |
| `refactor:` | Code refactoring |
| `test:` | Adding tests |
| `chore:` | Maintenance tasks |

### Example Commits
```bash
feat: Add OTA firmware updates CUL-300
fix: Resolve pump relay sticking issue CUL-301
docs: Update assembly guide with new PCB CUL-302
chore: Update ESP-IDF to v5.2 CUL-303
```

---

## 🔧 Troubleshooting

### Issue not linking?
- Ensure branch name contains issue ID (e.g., `CUL-123`)
- Check Linear integration is connected in GitLab
- Try adding issue ID in commit message

### Auto-close not working?
- Use exact keywords: `Closes`, `Fixes`, `Resolves`
- Issue ID must be in the commit merged to `main`
- Check Linear workflow automation settings

### Pipeline status not showing?
- Verify webhook is configured
- Check pipeline actually ran (not skipped)
- Ensure MR is linked to the issue

---

*Last Updated: December 2024*

