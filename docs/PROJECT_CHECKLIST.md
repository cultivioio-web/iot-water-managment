# Project Checklist & Roadmap

This document tracks major tasks, their completion status, and a phased rollout plan for the IoT Water Management System.

---

## Phase 0 – Environment & Tooling (Completed)
- [x] Create ESP32 test sketch (`test_setup.ino`) to verify Arduino setup
- [x] Provide flash erase utility (`FLASH_ERASE_FIX.ino`) for DFU recovery
- [x] Document upload checklist and troubleshooting flows

## Phase 1 – Core Single-Node Autonomy (Completed)
- [x] Implement dual pump logic (government priority + emergency backup)
- [x] Add seasonal thresholds, pump safety timeouts, and LED/buzzer alerts
- [x] Build HTTP setup interface (WiFi AP `WaterSystem-Setup`)
- [x] Document wiring and deployment (`README_ZIGBEE.md`, `WIRING_DIAGRAM.md`)
- [x] Deliver simplified firmware (`water_management_simplified.ino`)

## Phase 2 – Zigbee Wireless Foundation (In Progress)
- [x] Introduce duty-based configuration for multi-node roles
- [x] Implement baseline Zigbee cluster + custom attribute support
- [x] Align Zigbee firmware with single-node logic (alerts, pump wait timers)
- [x] Create ESP-IDF project (`firmware/zigbee_idf/`) for Zigbee builds
- [ ] **Multi-node status display** (LED reporting for remote nodes)
- [ ] **System testing** across at least two wireless nodes
- [ ] Expand Zigbee messaging (water level broadcast, pump commands)

## Phase 3 – Advanced Wireless Features (Future)
- [ ] Seamless node replacement & automatic failover
- [ ] Network health monitoring + heartbeat tracking
- [ ] Battery/solar telemetry for remote sensor nodes
- [ ] Cloud/edge hooks for remote dashboards (optional)

## Phase 4 – Monitoring & Quality Assurance
- [ ] End-to-end test plan covering firmware, backend (future), and mobile/web clients
- [ ] Automated regression tests / CI hooks (when repo adopts git)
- [ ] Deployment playbooks for tall-tank sites (integrate `TALL_TANK_DEPLOYMENT.md`)

---

### Current Focus Areas
1. Finish **Phase 2** items (multi-node status LEDs + Zigbee messaging).
2. Execute **system testing** checklist once two ESP32 nodes are available.
3. Begin Phase 3 planning for failover and telemetry once wireless foundation is validated.

Use this checklist to track progress and reprioritize tasks as hardware testing advances. Updates should be reflected both here and in the active TODO list.

