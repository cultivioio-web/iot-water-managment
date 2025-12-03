# Water Management System - Wiring Diagram

## Phase 1: ESP32-H2 Mini Configuration

This document shows the wiring for the two-node Zigbee water management system using ESP32-H2 Mini boards.

---

## System Overview

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                        WATER MANAGEMENT SYSTEM                              │
│                         Phase 1 Architecture                                │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│   ┌─────────────────────┐             ┌─────────────────────────┐           │
│   │   SENSOR NODE       │   Zigbee    │   CONTROLLER NODE       │           │
│   │   (ESP32-H2 Mini)   │◄═══════════►│   (ESP32-H2 Mini)       │           │
│   │                     │  802.15.4   │                         │           │
│   │  ┌─────────────┐    │             │    ┌─────────────┐      │           │
│   │  │ HC-SR04     │    │             │    │ Relay       │      │           │
│   │  │ Ultrasonic  │    │             │    │ Module      │──► PUMP          │
│   │  └─────────────┘    │             │    └─────────────┘      │           │
│   │                     │             │                         │           │
│   │  [Status LED]       │             │    [Status LED]         │           │
│   │  [Prov LED]         │             │    [Pump LED]           │           │
│   │  [Button]           │             │    [Button]             │           │
│   └─────────────────────┘             └─────────────────────────┘           │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## ESP32-H2 Mini Pinout Reference

```
                    ┌─────────────────┐
                    │   ESP32-H2      │
                    │     Mini        │
                    │   ┌───────┐     │
              3V3 ──┤   │       │     ├── GND
              GND ──┤   │  USB  │     ├── 5V (VBUS)
            GPIO0 ──┤   │   C   │     ├── GPIO27
            GPIO1 ──┤   │       │     ├── GPIO26
            GPIO2 ──┤   └───────┘     ├── GPIO25
            GPIO3 ──┤                 ├── GPIO24
            GPIO4 ──┤                 ├── GPIO23
            GPIO5 ──┤                 ├── GPIO22
            GPIO8 ──┤                 ├── GPIO14
            GPIO9 ──┤                 ├── GPIO13
           GPIO10 ──┤                 ├── GPIO12
           GPIO11 ──┤                 ├── GPIO11
                    └─────────────────┘
```

---

## Sensor Node Wiring

### Pin Assignments

| GPIO | Function | Connected To | Wire Color |
|------|----------|--------------|------------|
| GPIO2 | TRIG | HC-SR04 Trigger | Yellow |
| GPIO3 | ECHO | HC-SR04 Echo | Green |
| GPIO8 | STATUS_LED | Status LED (Anode) | Blue |
| GPIO9 | PROV_LED | Provisioning LED (Anode) | White |
| GPIO10 | BUTTON | Provisioning Button | Gray |
| 3V3 | Power | LED VCC (via resistor) | Red |
| 5V | Power | HC-SR04 VCC | Red |
| GND | Ground | Common Ground | Black |

### Wiring Diagram

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                          SENSOR NODE WIRING                                 │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│    ESP32-H2 Mini                    HC-SR04 Ultrasonic                      │
│    ┌──────────────┐                 ┌──────────────────┐                    │
│    │              │                 │                  │                    │
│    │         5V ──┼────────────────►│ VCC              │                    │
│    │        GND ──┼────────────────►│ GND              │                    │
│    │      GPIO2 ──┼────[Yellow]────►│ TRIG             │                    │
│    │      GPIO3 ──┼────[Green]─────►│ ECHO             │                    │
│    │              │                 │                  │                    │
│    │              │                 └──────────────────┘                    │
│    │              │                                                         │
│    │              │                 Status LED (Blue)                       │
│    │      GPIO8 ──┼────[220Ω]──────►│◄──► GND                               │
│    │              │                                                         │
│    │              │                 Provisioning LED (White)                │
│    │      GPIO9 ──┼────[220Ω]──────►│◄──► GND                               │
│    │              │                                                         │
│    │              │                 Provisioning Button                     │
│    │     GPIO10 ──┼─────────────────┤                                       │
│    │        GND ──┼─────────────────┤ (Normally Open)                       │
│    │              │                                                         │
│    └──────────────┘                                                         │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### Physical Layout - Sensor Node

```
        ┌─────────────────────────────────┐
        │         WATER TANK              │
        │    ┌───────────────────────┐    │
        │    │   ╔═══════════════╗   │    │    ◄── Mount sensor 5cm from top
        │    │   ║   HC-SR04     ║   │    │
        │    │   ║   ▼ ▼ ▼ ▼    ║   │     │    ◄── Sensor pointing down
        │    │   ╚═══════════════╝   │    │
        │    │          ║            │    │
        │    │    Sound Waves        │    │
        │    │          ║            │    │
        │    │          ▼            │    │
        │    │ ~~~~~~~~~~~~~~~~~~~~~~~~   │    │    ◄── Water Surface
        │    │                       │    │
        │    │                       │    │
        │    └───────────────────────┘    │
        │                                 │
        │    ┌───────────────────────┐    │
        │    │    ESP32-H2 Mini      │    │    ◄── Keep electronics dry!
        │    │    + LEDs + Button    │    │
        │    └───────────────────────┘    │
        │                                 │
        └─────────────────────────────────┘
```

---

## Controller Node Wiring

### Pin Assignments

| GPIO | Function | Connected To | Wire Color |
|------|----------|--------------|------------|
| GPIO2 | RELAY | Relay Module IN | Orange |
| GPIO8 | STATUS_LED | Status LED (Anode) | Blue |
| GPIO9 | PUMP_LED | Pump LED (Anode) | Red |
| GPIO10 | BUTTON | Provisioning Button | Gray |
| 5V | Power | Relay VCC | Red |
| GND | Ground | Common Ground | Black |

### Wiring Diagram

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                        CONTROLLER NODE WIRING                               │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│    ESP32-H2 Mini                    Relay Module                            │
│    ┌──────────────┐                 ┌──────────────────┐                    │
│    │              │                 │  ┌────┐          │                    │
│    │         5V ──┼────────────────►│  │    │ VCC      │                    │
│    │        GND ──┼────────────────►│  │RELY│ GND      │                    │
│    │      GPIO2 ──┼────[Orange]────►│  │    │ IN       │                    │
│    │              │                 │  └────┘          │                    │
│    │              │                 │                  │                    │
│    │              │                 │  NO ─────────────┼──► To Pump (+)     │
│    │              │                 │  COM ────────────┼──► From Power      │
│    │              │                 │  NC ─────────────┼──► (Unused)        │
│    │              │                 └──────────────────┘                    │
│    │              │                                                         │
│    │              │                 Status LED (Blue)                       │
│    │      GPIO8 ──┼────[220Ω]──────►│◄──► GND                               │
│    │              │                                                         │
│    │              │                 Pump LED (Red)                          │
│    │      GPIO9 ──┼────[220Ω]──────►│◄──► GND                               │
│    │              │                                                         │
│    │              │                 Provisioning Button                     │
│    │     GPIO10 ──┼─────────────────┤                                       │
│    │        GND ──┼─────────────────┤ (Normally Open)                       │
│    │              │                                                         │
│    └──────────────┘                                                         │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### Pump Connection (High Voltage - CAUTION!)

```
⚠️  WARNING: HIGH VOLTAGE - 220V AC ⚠️

┌─────────────────────────────────────────────────────────────────────────────┐
│                           PUMP POWER CIRCUIT                                │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│     AC MAINS (220V)              RELAY                    WATER PUMP        │
│                                                                             │
│        ┌─────┐                ┌────────┐               ┌──────────┐         │
│   L ───┤     ├───────────────►│  COM   │               │          │         │
│        │ MCB │                │        │               │   PUMP   │         │
│   N ───┤     ├────────────────┼────────┼──────────────►│   Motor  │         │
│        └─────┘                │   NO ──┼──────────────►│          │         │
│                               │        │               └──────────┘         │
│                               │   NC   │ (unused)                           │
│                               └────────┘                                    │
│                                                                             │
│   MCB = Miniature Circuit Breaker (10A recommended)                         │
│   Use proper wire gauge: 14 AWG minimum for pump                            │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## LED Connection Details

### LED Circuit with Current Limiting Resistor

```
                  220Ω Resistor
    GPIO ────────/\/\/\/\────────┤►├──────── GND
                                 LED
                            (Anode) (Cathode)

    LED Specifications:
    - Forward Voltage: 2.0V (Red), 3.0V (Blue/White)
    - Forward Current: 20mA max
    - Resistor Value: 220Ω (for 3.3V GPIO)
    
    Calculation: R = (Vgpio - Vled) / Iled
                 R = (3.3V - 2V) / 0.015A ≈ 86Ω (use 220Ω for safety)
```

### LED Color Codes

| LED | Color | Function | Behavior |
|-----|-------|----------|----------|
| Status | Blue | Connection Status | Solid = Connected, Blink = Waiting |
| Prov | White | Provisioning Mode | Fast Blink = Ready for BLE |
| Pump | Red | Pump Running | ON = Pump Active |

---

## Button Connection

### Provisioning Button (with Internal Pull-up)

```
    GPIO10 ──────────┬────────── Button ────────── GND
                     │
              (Internal Pull-up enabled in firmware)
    
    Button Operation:
    - Normally: GPIO10 reads HIGH (pull-up)
    - Pressed: GPIO10 reads LOW (connected to GND)
    - Hold 3 seconds: Enter provisioning mode
```

---

## Complete System Wiring

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                       COMPLETE SYSTEM DIAGRAM                               │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│   USB Power                USB Power                                        │
│      │                        │                                             │
│      ▼                        ▼                                             │
│ ┌─────────┐              ┌─────────┐                                        │
│ │ SENSOR  │   Zigbee     │CONTROL  │              ┌──────────┐              │
│ │  NODE   │◄════════════►│  NODE   │──────────────│  RELAY   │              │
│ │         │   Wireless   │         │              │  MODULE  │              │
│ └────┬────┘              └─────────┘              └────┬─────┘              │
│      │                                                 │                    │
│      │                                                 │                    │
│ ┌────┴────┐                                       ┌────┴─────┐              │
│ │ HC-SR04 │                                       │   PUMP   │              │
│ │ SENSOR  │                                       │   MOTOR  │              │
│ └─────────┘                                       └──────────┘              │
│      │                                                  │                   │
│      ▼                                                  │                   │
│ ┌─────────┐                                       ┌────┴─────┐              │
│ │  WATER  │                                       │  WATER   │              │
│ │  TANK   │◄──────────────────────────────────────│  SOURCE  │              │
│ └─────────┘            Water Flow                 └──────────┘              │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## Power Requirements

### Sensor Node
| Component | Voltage | Current | Power |
|-----------|---------|---------|-------|
| ESP32-H2 Mini | 5V | 150mA avg | 750mW |
| HC-SR04 | 5V | 15mA | 75mW |
| LEDs (x2) | 3.3V | 40mA | 132mW |
| **Total** | **5V** | **~200mA** | **~1W** |

### Controller Node
| Component | Voltage | Current | Power |
|-----------|---------|---------|-------|
| ESP32-H2 Mini | 5V | 150mA avg | 750mW |
| Relay Module | 5V | 80mA | 400mW |
| LEDs (x2) | 3.3V | 40mA | 132mW |
| **Total** | **5V** | **~270mA** | **~1.3W** |

### Recommended Power Supply
- **USB Power Adapter**: 5V 2A minimum
- **Or Battery**: 18650 Li-ion with 5V boost converter
- **Solar Option**: 6V 2W panel + TP4056 charger + 18650 battery

---

## Safety Checklist

### Before Powering On
- [ ] Double-check all wiring connections
- [ ] Verify no short circuits with multimeter
- [ ] Ensure relay contacts are wired correctly
- [ ] Check LED polarity (longer leg = anode = positive)
- [ ] Confirm button is normally open

### High Voltage Safety (Pump Circuit)
- [ ] Use appropriate wire gauge (14 AWG min)
- [ ] Install circuit breaker before pump
- [ ] Keep high voltage separated from low voltage
- [ ] Use proper insulation and enclosure
- [ ] Never work on live circuits
- [ ] Have a qualified electrician verify installation

---

## Troubleshooting

### Sensor Node Issues
| Problem | Possible Cause | Solution |
|---------|----------------|----------|
| No readings | Wrong pin connection | Check TRIG/ECHO wiring |
| Erratic readings | Sensor too close | Mount 5cm from water max |
| LED not lighting | Wrong polarity | Flip LED orientation |
| Button not working | Missing ground | Check button GND connection |

### Controller Node Issues
| Problem | Possible Cause | Solution |
|---------|----------------|----------|
| Relay not clicking | IN pin wrong | Check GPIO2 connection |
| Pump not starting | NO/COM swapped | Swap relay connections |
| Status LED off | Resistor missing | Add 220Ω resistor |
| No Zigbee | Too far apart | Move nodes closer |

---

**Document Version**: 2.0 (Phase 1 - ESP32-H2 Mini)  
**Last Updated**: December 2024
