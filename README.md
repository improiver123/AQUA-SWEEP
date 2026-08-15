# AQUA-SWEEP

### A Modular Robotic Platform for Floating-Waste Collection

**Robotics • Environmental Engineering • Distributed Systems**

---

## Abstract

AQUA-SWEEP is a low-cost floating robotic platform developed as a proof-of-concept for scalable aquatic-waste collection.

The prototype combines a **double-hull catamaran**, motorised propulsion, front-mounted waste collection, ESP32-based control and basic sensing.

The larger concept is to develop a **repeatable robotic base unit** that can be reproduced, networked and coordinated to address larger aquatic-cleanup tasks.

> **One robot is a prototype. Many AQUA-SWEEPs could become an environmental system.**

---

## 1. Context & Inspiration

The environmental motivation was influenced by **Team Seas**, associated with **MrBeast and Mark Rober**.

The project does not attempt to reproduce their system. Instead, the broader aquatic-cleanup challenge was abstracted into an independent engineering question:

> **Can large-scale aquatic cleanup be approached through many smaller, modular robots instead of one increasingly large machine?**

This question became the foundation of AQUA-SWEEP and its future **swarm / “AQUA-SWEEP Army”** concept.

---

## 2. Original Ideation

The central idea is:

```text
AQUATIC PROBLEM
      ↓
IDEA ABSTRACTION
      ↓
AQUA-SWEEP BASE UNIT
      ↓
REPLICATION
      ↓
COORDINATION
      ↓
DISTRIBUTED CLEANING
      ↓
LARGE-SCALE OPERATION
```

Instead of:

```text
BIGGER MACHINE
      ↓
MORE POWER
      ↓
MORE CAPACITY
```

AQUA-SWEEP proposes:

```text
BASE UNIT
    ↓
REPLICATE
    ↓
CONNECT
    ↓
COORDINATE
    ↓
SCALE
```

The physical prototype is the **first experimental embodiment of this broader concept**, developed through research, modelling, fabrication, testing and repeated modification.

---

## 3. Design Philosophy

AQUA-SWEEP was developed around five principles:

- **Stability** — double-hull catamaran architecture
- **Modularity** — independently adaptable subsystems
- **Accessibility** — low-cost and reused materials
- **Scalability** — repeatable base-unit architecture
- **Iteration** — build, test, modify and retest

---

## 4. Prototype Architecture

```text
                         AQUA-SWEEP
                              |
             +----------------+----------------+
             |                |                |
            HULL          PROPULSION       COLLECTION
             |                |                |
         CATAMARAN        MOTOR + DRIVE    WASTE INTAKE
             |
        ELECTRONICS
             |
       +-----+-----+------+
       |           |      |
     ESP32      SENSORS  POWER
       |
   CONTROL SYSTEM
```

The architecture separates the major functions so that individual systems can be modified and improved independently.

---

## 5. Current Prototype

The physical prototype demonstrates:

- Double-hull floating platform
- Motorised propulsion
- Front-mounted waste collection
- ESP32-based control
- L298N motor driver
- SG90 servo
- IR sensing
- LM2596 voltage regulation
- Rechargeable power system

### Principal Dimensions

| Parameter | Approximate Value |
|---|---:|
| Number of hulls | 2 |
| Hull diameter | 90 mm |
| Main PVC section | 240 mm |
| Overall hull length | 360 mm |
| Main deck | 200 × 200 mm |
| Hull configuration | Catamaran |

The hull and mechanical structure were refined through buoyancy testing, fabrication experiments and repeated modification.

---

## 6. Working Program & Control Algorithm

The complete ESP32 control program used for the AQUA-SWEEP prototype is provided in:

`./code/`

The firmware connects the ESP32 with the propulsion motor, motor driver, servo and sensors.

### Program Functions

- Motor and propulsion control
- Servo control
- Sensor input processing
- Direction and movement control
- Remote-control commands
- Basic control logic

### Control Algorithm

```text
START
  ↓
INITIALIZE ESP32
  ↓
INITIALIZE MOTORS, SERVO & SENSORS
  ↓
ESTABLISH CONTROL INTERFACE
  ↓
READ INPUT
  ↓
PROCESS COMMAND
  ↓
FORWARD / REVERSE / LEFT / RIGHT
  ↓
UPDATE MOTOR & SERVO
  ↓
READ SENSOR DATA
  ↓
UPDATE STATUS
  ↓
REPEAT LOOP
```

### Overall System Flow

```text
CONTROL INPUT
      ↓
    ESP32
      ↓
COMMAND PROCESSING
      ↓
+-----+------+
|            |
MOTOR       SERVO
CONTROL     CONTROL
|            |
PROPULSION  MECHANISM
      ↓
   SENSORS
      ↓
FEEDBACK / STATUS
      ↓
 CONTROL LOOP
      ↓
    REPEAT
```

The current firmware represents the working control architecture of the physical prototype. Future versions can extend it with GPS navigation, autonomous decision-making, computer vision and multi-robot communication.

**Input → ESP32 Processing → Actuation → Sensing → Feedback → Repeat**

## 7. Development Record

The project followed an iterative engineering process:

```text
PROBLEM
   ↓
IDEATION
   ↓
RESEARCH
   ↓
MODELLING
   ↓
FABRICATION
   ↓
BUOYANCY TESTING
   ↓
ELECTRONICS INTEGRATION
   ↓
TESTING
   ↓
MODIFICATION
   ↓
WORKING PROTOTYPE
```

A complete preparation and development video is documented on YouTube:

**[Watch the AQUA-SWEEP Development Video](https://youtu.be/3f8vH8szfpU?si=WTxRM3A5X1jHte3T)**

Development photographs are available in:

**[Project Images](./images/)**

---

## 8. AQUA-SWEEP Swarm / “Army” Concept

### The Future Vision

> **One AQUA-SWEEP is a prototype. Many AQUA-SWEEPs could become an environmental system.**

The long-term vision is to deploy multiple relatively simple robots across different areas of a water body.

```text
              [AS] [AS] [AS] [AS]
           [AS] [AS] [AS] [AS] [AS]
              [AS] [AS] [AS] [AS]

                       ↓
                SHARED INFORMATION
                       ↓
                 TASK ALLOCATION
                       ↓
                DISTRIBUTED CLEANING
                       ↓
                  GEO-MAPPED DATA
```

Future units could share:

- Position
- Waste concentration
- Battery status
- Environmental data
- Obstacles
- Collection capacity

This could enable adaptive deployment, cooperative navigation and distributed cleaning.

---

## 9. Geo-Mapping & Environmental Intelligence

A future AQUA-SWEEP network could combine positioning, sensing and visual information:

```text
ROBOT
  ↓
POSITION
  ↓
SENSOR DATA
  ↓
WASTE DETECTION
  ↓
GEO-TAGGING
  ↓
SHARED MAP
  ↓
CLEANING PRIORITY
  ↓
SWARM REDISTRIBUTION
```

This extends AQUA-SWEEP from a waste-collection platform toward a **distributed environmental-intelligence system**.

---

## 10. Future Architecture

### Autonomous Navigation

- GPS/GNSS
- Obstacle avoidance
- Autonomous path planning

### Intelligent Detection

- Camera-based waste detection
- Computer vision
- Waste-density estimation

### Environmental Monitoring

- Water-quality sensing
- Pollution mapping
- Geo-tagged environmental data

### Swarm Robotics

- Robot-to-robot communication
- Distributed task allocation
- Cooperative navigation
- Shared mapping

### Waste Processing

- Waste segregation
- Material classification
- Compaction
- Recycling transfer

These are **future concepts**, not claimed capabilities of the current prototype.

---

## 11. Innovation & Intellectual-Property Potential

AQUA-SWEEP is documented as an independently conceived and developed engineering project.

Its innovation direction combines:

**stable aquatic robotics + modular waste collection + embedded control + repeatable base-unit architecture + scalable multi-robot deployment.**

Potential intellectual-property investigation may focus on specific implementations involving:

- Mechanical collection systems
- Modular hull and collection arrangements
- Robotic deployment methods
- Distributed waste mapping
- Swarm task allocation
- Cooperative environmental cleaning

The repository **does not claim that a patent has already been granted or that every aspect is patentable**. Formal prior-art and patentability assessment would be required before filing.

This repository serves as a technical record of the project's concept, development and implementation.

---

## 12. Prototype vs Future

| Capability | Current Prototype | Future Concept |
|---|:---:|:---:|
| Catamaran platform | ✓ | |
| Propulsion | ✓ | |
| Waste collection | ✓ | |
| ESP32 control | ✓ | |
| Basic sensing | ✓ | |
| GPS/GNSS | | ✓ |
| Computer vision | | ✓ |
| Geo-mapping | | ✓ |
| Robot-to-robot communication | | ✓ |
| AQUA-SWEEP swarm | | ✓ |
| Integrated recycling | | ✓ |
| Large-scale deployment | | ✓ |

---

## 13. Prototype Limitations

The current system is an engineering **proof-of-concept**, not a production-ready autonomous marine platform.

Current limitations include:

- Limited physical scale
- Limited energy capacity
- Basic propulsion architecture
- Prototype-level waterproofing
- Basic sensing capability
- Limited autonomous functionality
- Prototype-level waste storage
- No validated swarm operation
- No implemented geo-mapping network
- No implemented recycling-processing system

These limitations define important areas for future research.

---

## 14. Future Research Directions

### Mechanical

- Improved hull hydrodynamics
- Better propulsion efficiency
- Improved waterproofing
- Modular collection mechanisms
- Improved structural strength

### Electronics

- Custom PCB
- Better power management
- Battery-management system
- Expanded sensing
- Wireless communication

### Autonomous Systems

- GPS navigation
- Obstacle avoidance
- Path planning
- Computer vision
- Automatic waste detection

### Environmental Intelligence

- Water-quality sensing
- Turbidity measurement
- Pollution mapping
- Environmental data logging

### Swarm Robotics

- Inter-robot communication
- Distributed task allocation
- Collision avoidance
- Dynamic zone assignment
- Shared mapping

### Waste Processing

- Automatic segregation
- Material classification
- Compaction
- Temporary storage
- Recycling integration

---

## 15. Bill of Materials

Major components and materials include:

- 90 mm PVC pipe
- 40 mm PVC pipe
- Acrylic board
- Reused 2-litre plastic bottles
- ESP32 development board
- L298N motor driver
- High-speed propulsion motor and propeller
- SG90 servo
- IR sensors
- 3 × 3.7 V 1200 mAh rechargeable cells
- 9 V batteries and connectors used during experimentation
- LM2596 voltage regulator
- Resistors and capacitors
- 400-pin breadboard
- Switch
- M-Seal
- Fevicol
- Feviquick
- Hot glue
- Steel wire
- Connecting wires
- Paint and brushes
- Digital multimeter

---

## 16. Author

**Krishna Deka**  
C/O Prof. U.S. Dixit  
IIT Guwahati

AQUA-SWEEP was developed as a self-directed engineering project through research, design exploration, fabrication, electronics integration, testing and future-system ideation.

---

## 17. Final Vision

The ultimate objective is not limited to building a single floating waste collector.

The broader vision is a scalable family of environmental robots:

```text
SMALL WATER BODY
      ↓
FEW AQUA-SWEEPS
      ↓
LOCAL CLEANING

MEDIUM WATER BODY
      ↓
MULTIPLE AQUA-SWEEPS
      ↓
DISTRIBUTED CLEANING

LARGE WATER BODY
      ↓
AQUA-SWEEP SWARM
      ↓
GEO-MAPPED CLEANING
      ↓
DATA + MONITORING
```

The fundamental architecture remains:

```text
BUILD THE UNIT
      ↓
REPRODUCE THE UNIT
      ↓
CONNECT THE UNITS
      ↓
COORDINATE THE UNITS
      ↓
SCALE THE SYSTEM
```

---

# AQUA-SWEEP

### From one floating robot to a scalable network of cooperating environmental robots.

**Concept → Abstraction → Prototype → Replication → Coordination → Scale**
