# Water Management System - Architecture Discussion

## Overview
A 3-node self-sufficient water management system using Zigbee mesh networking with ESP32 devices. The system prioritizes government water supply while providing emergency backup, operating completely autonomously after initial setup.

## Core Requirements (Based on User Preferences)
- **3 ESP32 nodes** in a Zigbee mesh network (50ft range)
- **Configurable fixed roles**: Roles assigned at setup time, remain fixed during operation
- **Self-sufficient operation**: No internet/cloud dependency after setup
- **Priority-based logic**: Government water > Emergency pump
- **Timing intelligence**: Learn and predict government water patterns
- **Seasonal operation**: Different logic for monsoon vs dry seasons
- **Alert system**: LEDs and buzzer only (no mobile alerts)
- **Power options**: Battery and mains power available
- **Set-and-forget**: Minimal maintenance, automatic operation

## Architecture Options

### Selected: Configurable Fixed Roles (Based on User Requirements)

```
Setup Time Configuration (via web interface):
├── Node 1: [Selectable] Primary Sensor / Secondary Sensor / Pump Controller
├── Node 2: [Selectable] Main Controller / Backup Controller / Valve Controller
└── Node 3: [Selectable] Backup Sensor / Alert System / Power Monitor

Runtime Operation (Fixed roles after setup):
├── Roles remain constant during autonomous operation
├── No dynamic role changes during normal operation
└── Clear responsibility assignment for reliability
```

**Role Options:**
- **Primary Sensor**: Water level measurement, government water detection
- **Secondary Sensor**: Backup measurements, environmental monitoring
- **Main Controller**: Decision making, emergency logic, coordination
- **Backup Controller**: Failover decision making, system health monitoring
- **Pump Controller**: Submersible pump activation and monitoring
- **Valve Controller**: Additional valve controls if needed
- **Alert System**: LED indicators, buzzer alerts, status reporting

**Pros**: Clear responsibilities, predictable behavior, easier debugging
**Cons**: Less flexible than dynamic roles, requires careful initial configuration

## Recommended Architecture: Flexible Mesh

### Node Capabilities
Each ESP32 node can be configured for multiple roles simultaneously:

**Sensor Roles:**
- Ultrasonic water level measurement
- Government water flow detection
- Temperature monitoring
- System health monitoring

**Controller Roles:**
- Decision making engine
- Pump activation logic
- Emergency response coordination
- Timing pattern analysis

**Actuator Roles:**
- Submersible pump control
- Valve control (if needed)
- Alert system (buzzer/LED)
- Status indicators

### Communication Protocol
- **Zigbee Mesh**: For reliable device-to-device communication
- **No internet required**: All communication stays local
- **Setup only**: HTTP web interface for initial configuration
- **Self-healing mesh**: Automatic route discovery and failover

## Design Philosophy

### 1. **Reliability First**
- Triple redundancy: If one node fails, others compensate
- Graceful degradation: System continues with reduced capability
- Self-diagnostic capabilities
- Automatic error recovery

### 2. **Energy Efficiency**
- Deep sleep modes when possible
- Wake-on-sensor events
- Optimized measurement intervals
- Low-power Zigbee communication

### 3. **Simplicity in Operation**
- Zero-touch after setup
- Automatic learning of government water patterns
- Predictive emergency activation
- Minimal user intervention required

### 4. **Flexible Deployment**
- Any node can take any role
- Easy reconfiguration without hardware changes
- Modular hardware design
- Software-defined functionality

## Working Principles

### Phase 1: Setup & Configuration (HTTP Required)
1. Connect all 3 nodes to same WiFi network
2. Access web interface on any node (192.168.1.100/setup)
3. Configure:
   - Tank dimensions and thresholds
   - Node roles (fixed for each node)
   - Seasonal settings (monsoon/dry season logic)
   - Alert preferences (LED patterns, buzzer settings)
   - Power management options
4. Test system functionality and communication
5. Disable WiFi after setup (becomes standalone Zigbee mesh)

### Phase 2: Autonomous Operation (Self-Sufficient Zigbee Mesh)

#### Normal Monitoring
- Water level measurement (interval based on season)
- Government water detection scanning
- Pattern learning and prediction
- Health status monitoring
- Seasonal logic adjustment

#### Seasonal Operation Logic
```
Monsoon Season (High govt water frequency):
├── More aggressive waiting for govt water (up to 4 hours)
├── Emergency pump threshold: 15% (higher tolerance)
├── Learning period: 3 days (rapid pattern recognition)
└── Alert frequency: Reduced (normal in monsoon)

Dry Season (Low govt water frequency):
├── Conservative waiting for govt water (1-2 hours)
├── Emergency pump threshold: 10% (lower tolerance)
├── Learning period: 7 days (extended pattern analysis)
└── Alert frequency: Increased (critical in dry season)
```

#### Emergency Logic
```
Water Level < Threshold (10% dry / 15% monsoon):
├── Check seasonal govt water prediction
├── Wait for predicted time window (±2 hours)
├── If govt water detected: Allow natural filling to 90%
├── If timeout: Activate emergency pump to 60%
└── Stop at target or govt water detection
```

#### Government Water Learning
- Records timestamps with seasonal context
- Analyzes patterns by season (different schedules)
- Predicts next likely water time based on season
- Maintains separate pattern databases for each season

#### Failover & Recovery
- If primary sensor fails: Secondary sensor takes over
- If main controller fails: Backup controller activates
- If communication lost: Nodes operate independently with last known parameters
- Automatic health checks and status reporting

## Alert System

### Local Alerts Only (No Mobile)
- **LED Indicators**:
  - Green: Normal operation
  - Yellow: Low water warning
  - Red: Emergency pump active
  - Blue: Government water detected
  - Flashing Red: System error/fault

- **Buzzer Alerts**:
  - Short beep: Status update
  - Long beep: Emergency activation
  - Rapid beeping: System fault
  - Patterned beeps: Seasonal change acknowledgment

- **Alert Logic**:
  - Seasonal alert frequency adjustment
  - Acknowledgment system (button press to silence)
  - Escalating alert patterns for persistent issues

## Power Management

### Dual Power Options
- **Mains Power**: Preferred for controller nodes
- **Battery Power**: Available for sensor nodes, with solar charging option

### Sleep Strategies (Battery Optimization)
- **Deep Sleep**: 25 minutes of 30-minute cycle (battery nodes)
- **Light Sleep**: During active monitoring periods
- **Active**: Only during measurements, decisions, and alerts

### Wake Triggers
- Timer-based measurements (seasonal intervals)
- Sensor interrupts (water level critical changes)
- Zigbee network messages from other nodes
- Emergency conditions requiring immediate action
- Alert acknowledgment button presses

## Error Handling

### Detection
- Sensor reading validation
- Communication heartbeat monitoring
- Power supply monitoring
- Pump operation verification

### Recovery
- Automatic sensor recalibration
- Communication route rediscovery
- Emergency mode activation
- User alert via LED patterns

## Configuration Flexibility

### Node Role Combinations
```
Sensor Only: Water level + govt detection
Controller Only: Decision making + coordination
Actuator Only: Pump/valve control
Sensor + Controller: Local decision making
Controller + Actuator: Coordinated control
Sensor + Actuator: Direct response
All Roles: Full capability node
```

### Dynamic Reconfiguration
- Runtime role changes via Zigbee commands
- Automatic role reassignment on failure
- Load balancing across capable nodes
- Seasonal adjustment (different logic for rainy/dry seasons)

## Implementation Considerations

### Hardware Requirements per Node
- ESP32 with Zigbee module
- Ultrasonic sensor (HC-SR04)
- Flow/pressure sensor (govt water)
- Relay module (pump control)
- Power supply (battery + solar option)
- Status LEDs and buttons

### Software Architecture
- **Modular design**: Separate concerns for sensor, control, actuation
- **State machines**: Clear operational states and transitions
- **Event-driven**: Respond to sensor events and network messages
- **Configuration storage**: Local preferences for setup parameters

### Testing Strategy
- Individual node testing
- Mesh network testing
- Failure scenario simulation
- Long-term reliability testing

## Agreed Architecture Summary (Updated for Dynamic Network)

Based on your latest requirements, we've evolved to a dynamic network system:

✅ **Dynamic Duty Assignment**: 1-3 nodes, duties distributed based on network size
✅ **Single Node Mode**: One node performs ALL duties (sensor + controller + actuator)
✅ **Multi-Node Coordination**: 2-3 nodes work together with assigned duties
✅ **Status Sharing**: Every node displays status of ALL nodes in network
✅ **Seamless Failover**: Replace failed nodes, new nodes inherit duties automatically
✅ **Self-Aware Network**: System detects network topology and adapts accordingly
✅ **Communication**: Zigbee mesh network (50ft range)
✅ **Power**: Battery and mains power options available
✅ **Alerts**: LED indicators and buzzer only (no mobile alerts)
✅ **Seasonal Logic**: Different operation for monsoon vs dry seasons
✅ **Self-Sufficient**: No internet/cloud dependency after setup

### Dynamic Network Operation:

**1 Node Network:**
- Single node performs ALL duties
- Self-contained water management system
- Controls both government and alternative water pumps
- All sensors, control logic, and actuation on one device

**2 Node Network:**
- **Node A**: Sensor duties + government pump control
- **Node B**: Main control + alternative pump control
- Coordinated operation with status sharing

**3 Node Network:**
- **Node A**: Primary sensing (water level, govt water detection)
- **Node B**: Government water supply pump control + main decision making
- **Node C**: Emergency pump control + alert system + backup sensing
- Full redundancy with dual water source management

### Key Features:
- HTTP setup interface for duty assignment only
- Zigbee mesh with automatic network discovery
- Real-time status sharing between all nodes
- Seamless node replacement and duty inheritance
- Seasonal water pattern learning
- Emergency pump activation at 10% (dry) / 15% (monsoon)
- Local LED/buzzer alerts with multi-node status display
- Battery/mains power flexibility

## Ready for Implementation?

The architecture is now finalized. Shall we proceed with implementing the 3-node Zigbee water management system?

The implementation will include:
1. ESP32 firmware with Zigbee support
2. Web-based setup interface (HTTP)
3. Seasonal logic and pattern learning
4. Self-sufficient autonomous operation

Would you like to start implementation, or do you have any questions about the finalized architecture?
