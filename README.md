# AQUA-SWEEP

### A Modular Robotic Platform for Floating-Waste Collection

**Research • Robotics • Environmental Engineering • Sustainable Technology**

---

## Abstract

AQUA-SWEEP is a research-driven engineering prototype developed to explore a low-cost, modular approach to floating-waste collection.

The project investigates how a small floating robotic platform can be designed around stability, modularity, propulsion, waste collection, embedded electronics and sensing, while keeping the underlying architecture simple enough to be reproduced and scaled.

The physical prototype uses a double-hulled catamaran configuration fabricated primarily from PVC pipe sections, recycled plastic components and lightweight structural materials. The catamaran architecture was selected with stability as a major design consideration, providing a wider support base than a conventional single-hull platform.

Beyond the individual prototype, AQUA-SWEEP proposes a larger scalable concept: instead of relying exclusively on a single large waste-collection machine, multiple smaller robotic units could operate cooperatively across a water body. Such a system could potentially combine distributed cleaning, sensing, geo-mapping, camera-based monitoring and coordinated deployment.

The present work therefore represents both a physical proof-of-concept and an engineering framework for investigating scalable multi-robot aquatic-cleanup systems.

---

## 1. Project Motivation

The motivation for AQUA-SWEEP originated from the broader challenge of aquatic pollution and large-scale floating-waste removal.

The project was inspired by public environmental initiatives such as **Team Seas**, associated with MrBeast and Mark Rober, and the wider idea of using technology and collective action to address pollution in water bodies.

Rather than attempting to reproduce an existing large-scale cleanup system, the project asks a different engineering question:

> **Can the fundamental idea of floating-waste collection be translated into a small, low-cost robotic platform that can eventually be deployed as multiple cooperating units?**

This question became the foundation of the AQUA-SWEEP concept.

The project was developed through independent research, modelling, material selection, fabrication, experimentation, failure analysis and repeated modification.

---

## 2. Research Objective

The primary objective of AQUA-SWEEP is to investigate the feasibility of a compact floating robotic platform capable of:

- Maintaining stability on water
- Moving through a water body
- Collecting floating waste
- Carrying embedded electronics
- Supporting sensors and control hardware
- Using inexpensive and accessible construction methods
- Providing a platform that can be further automated
- Serving as a foundation for multiple-robot deployment

A secondary objective is to investigate whether a collection of relatively small robotic units could provide a more flexible and scalable alternative to a single large machine.

---

## 3. Design Philosophy

The design was developed around five major principles:

### Stability
A double-hulled catamaran configuration was selected to provide a broad support base and improve resistance to unwanted rolling.

### Modularity
The platform is designed so that propulsion, sensing, collection and control systems can be modified independently.

### Low-Cost Fabrication
The prototype prioritises commonly available materials such as PVC pipe, plastic bottles, acrylic sheet, wires, adhesives and inexpensive electronic modules.

### Scalability
The physical prototype is considered a base unit rather than the final form of the system.

### Iterative Development
The prototype was produced through repeated experimentation, buoyancy testing, hit-and-trial fabrication and mechanical/electrical modification.

---

# 4. Prototype Architecture

The AQUA-SWEEP prototype consists of several interconnected subsystems:

```text
                                                   AQUA-SWEEP
                                                        |
                                        +---------------+---------------+
                                        |               |               |
                                    Hull System     Propulsion      Collection
                                        |               |               |
                                    Catamaran       Motor + Drive    Waste Intake
                                        |
                                    Electronics
                                        |
                                  +-----+-----+------+
                                  |           |      |
                                ESP32      Sensors  Power
                                  |
                            Control / Communication
```

---

# 5. Hull Design

The hull of AQUA-SWEEP was developed using a double-hulled catamaran configuration. The design was primarily selected with stability as a major consideration while providing sufficient space for the propulsion, collection, electronics and power systems.

The primary hull structure uses two approximately 90 mm diameter PVC pipes. Each main PVC section has a length of approximately 240 mm. Additional length and shaping were achieved through sectioning and adapting 2-litre plastic bottles.

The resulting overall hull length is approximately 360 mm.

### Principal Hull Dimensions

| Parameter | Approximate Value |
|---|---:|
| Number of hulls | 2 |
| Hull material | PVC pipe |
| Hull diameter | 90 mm |
| Main PVC section length | 240 mm |
| Approximate overall hull length | 360 mm |
| Main deck | 200 × 200 mm |
| Nose inclination | Approximately 45° |
| Hull configuration | Double-hull catamaran |

The two buoyant hulls are positioned parallel to each other and connected through the central deck structure. This arrangement provides a wider support base than a single-hull configuration and was selected primarily to improve stability.

The hull geometry was developed through repeated experimentation, buoyancy testing and physical modification.

---

# 6. Catamaran Stability Concept

Stability was one of the primary design considerations throughout the development of AQUA-SWEEP.

The double-hull catamaran arrangement provides two separated buoyant bodies with the main deck positioned between them. This increases the effective width of the floating platform and helps distribute the weight of the onboard systems.

The design therefore focuses on:

- Increased lateral stability
- Distribution of the payload across two hulls
- Central placement of the main deck
- Accommodation of the collection mechanism
- Space for electronics and power components
- A modular structure that can be modified during development

Rather than treating the hull as a fixed geometry from the beginning, the final form was obtained through repeated testing and modification.

---

# 7. Main Deck

The main deck forms the central structural platform of AQUA-SWEEP.

The deck was fabricated using an acrylic board with an approximate dimension of **200 × 200 mm**.

The deck provides the mounting area for the electronic components, wiring and mechanical assemblies.

The central deck also acts as the connection between the two hulls and provides a practical location for distributing the onboard components.

Component placement was adjusted during fabrication according to available space, accessibility and overall stability considerations.

---

# 8. Hull Nose Design

The front section of the hull was developed with an approximately **45° inclined nose geometry**.

The nose was progressively shaped and reinforced during fabrication. Sectioned 2-litre plastic bottles were used to provide additional hull length and to help form the required geometry.

M-Seal was used for sealing and finishing the nose region.

The nose structure was developed through repeated physical modification rather than being treated as a completely fixed design from the beginning.

---

# 9. Low-Cost and Recycled Construction

A major consideration during the development of AQUA-SWEEP was the use of accessible and relatively inexpensive materials.

The prototype incorporates materials including:

- PVC pipe
- Reused 2-litre plastic bottles
- Acrylic sheet
- Steel wires
- Adhesives
- M-Seal
- Hot glue
- Lightweight structural materials

The use of recycled plastic bottles was particularly useful for extending and shaping the hull while reducing the requirement for additional structural material.

This construction approach also demonstrates how commonly available materials can be adapted during early-stage robotic prototyping.

---

# 10. Propulsion System

The AQUA-SWEEP prototype uses a high-speed propulsion motor to provide forward and backward movement.

The propulsion subsystem includes:

- High-speed propulsion motor
- Main propulsion propeller
- L298N motor driver
- Motor mounting arrangement
- Mechanical shaft arrangement
- Power wiring

The L298N motor driver provides the electrical interface between the control electronics and propulsion motor.

The motor and associated mechanical components were mounted within the available hull space and adjusted during fabrication to obtain a practical propulsion arrangement.

The propulsion system represents one of the major subsystems requiring further optimisation in future versions.

---

# 11. Electronics Architecture

The electronic control system is centred around an ESP32 development board.

The major electronic components used during the prototype development include:

- ESP32 development board
- L298N motor driver
- SG90 servo
- IR sensors
- LM2596 voltage regulator
- Rechargeable battery cells
- 9 V batteries used during experimentation
- Battery connectors
- 4.7 kΩ resistors
- 100 nF capacitors
- 470 µF capacitors
- 400-pin breadboard
- Switch
- Connecting wires

The ESP32 provides the programmable control platform for future development of wireless communication, sensing and autonomous functions.

---

# 12. Power System

The prototype power system incorporates rechargeable cells together with supporting voltage-regulation components.

The development included:

- Three 3.7 V, 1200 mAh rechargeable cells
- Three 9 V batteries used during experimentation
- Three 9 V battery connectors
- LM2596 voltage regulator
- Power switch
- Capacitors for voltage stabilisation

The LM2596 module was used as part of the voltage-regulation system during development.

A digital multimeter was used to measure and verify electrical parameters while working with the voltage-regulation system.

---

# 13. Sensor System

IR sensors were incorporated into the prototype electronics for sensing and experimental control purposes.

The electronics development also included:

- 4.7 kΩ resistors
- 100 nF capacitors
- 470 µF capacitors

These components formed part of the experimental electronics setup.

The current sensor configuration represents a foundation for future expansion.

Potential future sensing capabilities include:

- Ultrasonic distance sensing
- Temperature sensing
- Water-quality sensing
- GPS/GNSS positioning
- Camera-based monitoring
- IMU-based orientation sensing
- Floating-waste detection

These are proposed future capabilities and should not be considered fully implemented features of the current prototype.

---

# 14. Waste Collection Mechanism

The AQUA-SWEEP prototype incorporates a front-mounted waste-collection structure designed to intercept floating waste and guide it towards the collection region.

The collection structure was fabricated using lightweight and readily available materials.

The mechanism was intentionally kept relatively simple during the prototype stage so that it could be modified and tested without requiring a complete redesign of the platform.

Future collection mechanisms can be adapted according to:

- Waste size
- Waste density
- Water conditions
- Required collection rate
- Robot size
- Available storage capacity

The modular structure allows the collection mechanism to be redesigned independently from the primary hull.

---

# 15. Proposed Integrated Recycling System

A future development direction for AQUA-SWEEP is an integrated onboard recycling or waste-processing system.

This is a **proposed future feature and is not claimed as fully implemented in the current prototype**.

A conceptual system could follow:

```text
                                              Detection
                                                  ↓
                                              Collection
                                                  ↓
                                              Separation
                                                  ↓
                                              Processing / Compaction
                                                  ↓
                                              Temporary Storage
                                                  ↓
                                              Transfer for Recycling
                                              ```

The objective would be to move beyond simple collection towards a more complete floating waste-management platform.
```

---

# 16. Prototype Development Methodology

The AQUA-SWEEP prototype was developed through an iterative engineering process involving research, modelling, fabrication, testing and modification.

The development cycle can be represented as:

```text
                                            Problem Identification
                                                    ↓
                                            Initial Concept
                                                    ↓
                                            Research
                                                    ↓
                                            Material Selection
                                                    ↓
                                            Modelling
                                                    ↓
                                            Fabrication
                                                    ↓
                                            Buoyancy Testing
                                                    ↓
                                            Observation
                                                    ↓
                                            Modification
                                                    ↓
                                            Retesting
                                                    ↓
                                            Prototype Refinement
```

The physical model was developed through repeated experimentation and hit-and-trial fabrication.

Changes to one subsystem frequently affected another subsystem. For example, changes in hull dimensions affected stability, component placement affected weight distribution, and propulsion placement affected the available space for the collection mechanism.

This iterative process became an important part of the engineering methodology.

---

# 17. Research Perspective

From a research perspective, AQUA-SWEEP is not limited to the construction of a single floating robot.

The larger research question is:

> **How can a relatively simple floating robotic platform become the base unit of a scalable environmental robotics system?**

Instead of attempting to immediately construct one highly complex autonomous machine, the project follows a bottom-up development philosophy.

A small platform can first demonstrate:

- Buoyancy
- Stability
- Propulsion
- Waste collection
- Basic sensing
- Electronic control

Once these fundamental functions are validated, increasingly advanced capabilities can be introduced.

This approach reduces development complexity and allows individual subsystems to be tested independently.

---

# 18. From One Robot to a Swarm

The long-term vision of AQUA-SWEEP is to move from a single robotic platform towards a coordinated group of smaller robots.

The concept is based on distributed robotic systems in which multiple relatively simple agents can collectively perform a larger task.

Instead of constructing one extremely large machine, a water body could potentially be divided into operational zones and multiple AQUA-SWEEP units could be deployed simultaneously.

```text
                                                          WATER BODY
                                      
                                             [AS-01]       [AS-02]       [AS-03]
                                      
                                                  [AS-04]       [AS-05]
                                      
                                             [AS-06]       [AS-07]       [AS-08]
                                      
                                                          ↓
                                      
                                                   CENTRAL SYSTEM
                                      
                                              Mapping | Monitoring
                                              Control | Data Analysis
```

Each unit could operate as an individual cleaning platform while contributing information to a larger network.

---

# 19. AQUA-SWEEP Swarm Concept

The proposed swarm system could allow multiple AQUA-SWEEP units to share information such as:

- Position
- Direction
- Waste concentration
- Battery status
- Operating status
- Collection capacity
- Environmental measurements

The system could then redistribute robots according to observed conditions.

For example:

```text
                                            Low Waste Zone
                                                  ↓
                                            Fewer Robots
                                            
                                            High Waste Zone
                                                  ↓
                                            More Robots
                                            
                                            Obstacle / Restricted Zone
                                                  ↓
                                            Robots Redirected
```

This would allow the overall system to become adaptive rather than relying entirely on predetermined paths.

The central idea is therefore **distributed cleaning rather than dependence on one large cleaning machine**.

---

# 20. Geo-Mapping Concept

Future AQUA-SWEEP units could incorporate GPS/GNSS positioning together with cameras and environmental sensors to develop a spatial representation of a water body.

A conceptual workflow would be:

```text
                                              Robot Deployment
                                                      ↓
                                              Position Acquisition
                                                      ↓
                                              Sensor Measurements
                                                      ↓
                                              Waste Detection
                                                      ↓
                                              Geo-Tagging
                                                      ↓
                                              Water-Body Map
                                                      ↓
                                              Cleaning Priority Map
                                                      ↓
                                              Swarm Redistribution
```

The resulting map could identify areas with:

- High waste concentration
- Low waste concentration
- Repeated waste accumulation
- Navigation restrictions
- Potential pollution sources

This would transform AQUA-SWEEP from a basic cleaning platform into a potential mobile environmental data-collection system.

---

# 21. Camera-Based Monitoring

Future versions of AQUA-SWEEP could incorporate compact camera modules.

Camera-equipped units could potentially support:

- Visual monitoring
- Floating-waste identification
- Obstacle detection
- Remote inspection
- Water-body observation
- Environmental documentation
- Data collection for future computer-vision systems

A future computer-vision system could potentially assist in distinguishing floating waste from other objects.

This is a proposed future capability and is not claimed as fully implemented in the current prototype.

---

# 22. Army of AQUA-SWEEP Robots

The ultimate conceptual direction of the project is the deployment of an **AQUA-SWEEP swarm**.

The term "army" is used conceptually to describe a large coordinated population of small environmental robots.

Each robot would act as an individual unit with a limited set of capabilities.

Collectively, however, the units could perform a much larger environmental-cleaning task.

```text
                                                     AQUA-SWEEP SWARM
                                    
                                                    [AS] [AS] [AS] [AS]
                                          
                                                 [AS] [AS] [AS] [AS] [AS]
                                          
                                                    [AS] [AS] [AS] [AS]
                                          
                                                [AS] [AS] [AS] [AS] [AS]
                                          
                                                            ↓
                                        
                                                     WATER BODY COVERAGE
                                        
                                                            ↓
                                        
                                                     DISTRIBUTED CLEANING
                                        
                                                            ↓
                                        
                                                      GEO-MAPPED RESULTS
```

The central research idea is:

> **Scalability can come from replication rather than continuously increasing the size and complexity of one machine.**

A large number of relatively simple units could potentially be deployed across different sections of a water body and reorganised according to environmental conditions.

---

# 23. Scalability

AQUA-SWEEP is envisioned as a scalable platform that can develop through several stages.

### Level 1 — Prototype

One manually controlled floating AQUA-SWEEP unit.

### Level 2 — Assisted Operation

One unit with improved sensing and semi-autonomous navigation.

### Level 3 — Multiple Units

Several units operating within the same water body.

### Level 4 — Coordinated Swarm

Multiple units sharing positional and environmental information.

### Level 5 — Large-Scale Environmental Network

Multiple coordinated fleets operating across different water bodies.

This staged approach avoids introducing every advanced feature simultaneously and provides a practical development pathway.

---

# 24. Simplified Future Implementation Strategy

A major engineering principle for future development is to keep implementation modular and progressively increase complexity.

The recommended development pathway is:

### Stage 1
Remote-controlled propulsion and waste collection.

### Stage 2
Add reliable sensing.

### Stage 3
Add autonomous obstacle avoidance.

### Stage 4
Add GPS-based navigation.

### Stage 5
Add waste-density mapping.

### Stage 6
Add communication between robots.

### Stage 7
Implement swarm coordination.

### Stage 8
Integrate advanced waste segregation and recycling.

This staged approach allows individual subsystems to be validated before they become part of a more complex integrated system.

---

# 25. Prototype Limitations

The current AQUA-SWEEP prototype is an engineering proof-of-concept and should not be considered a production-ready autonomous marine robot.

The present prototype has several limitations:

- Limited physical scale
- Limited energy capacity
- Basic propulsion architecture
- Prototype-level waterproofing
- Limited sensing capability
- Limited autonomous functionality
- Prototype-level waste storage
- No validated large-scale swarm operation
- No demonstrated autonomous geo-mapping network
- No fully implemented recycling-processing system

These limitations also define important areas for future research and development.

---

# 26. Future Research Directions

Future research can focus on several major areas.

## Mechanical Development

- Improved hull hydrodynamics
- Lightweight structural materials
- Improved propulsion efficiency
- Modular collection mechanisms
- Improved waterproofing
- Improved structural strength

## Electronics

- Custom PCB development
- Improved power management
- Battery-management system
- Expanded sensor integration
- Wireless communication
- Better protection of electronics from water

## Autonomous Navigation

- Obstacle avoidance
- GPS navigation
- Path planning
- Return-to-base functionality
- Automatic waste detection

## Environmental Monitoring

- Water-quality sensing
- Temperature measurement
- Turbidity measurement
- Pollution mapping
- Environmental data logging

## Swarm Robotics

- Inter-robot communication
- Distributed task allocation
- Collision avoidance
- Dynamic zone assignment
- Distributed mapping
- Adaptive deployment

## Waste Processing

- Automatic segregation
- Waste compaction
- Temporary storage
- Transfer to collection stations
- Future recycling integration

---

# 27. Project Significance

AQUA-SWEEP explores an alternative approach to aquatic-cleanup robotics.

Instead of treating the cleaning machine as one large system, the project considers the possibility of developing a distributed robotic ecosystem.

The central idea is:

> **A simple robot becomes significantly more capable when many such robots can cooperate.**

The physical prototype provides the foundation for investigating this concept.

The broader objective is therefore not simply to build a boat, but to explore how low-cost robotics, environmental sensing, distributed systems and scalable deployment can be combined into a future water-cleanup architecture.

---

# 28. Prototype vs Proposed System

To maintain technical clarity, the project distinguishes between features demonstrated in the current prototype and features proposed for future development.

| Feature | Current Prototype | Future Concept |
|---|:---:|:---:|
| Double-hull catamaran | ✓ | |
| Floating platform | ✓ | |
| Propulsion | ✓ | |
| Waste collection | ✓ | |
| ESP32-based electronics | ✓ | |
| Servo-based control | ✓ | |
| IR sensing | ✓ | |
| GPS geo-mapping | | ✓ |
| Advanced autonomous navigation | | ✓ |
| Embedded camera network | | ✓ |
| Multi-robot swarm | | ✓ |
| Dynamic swarm coordination | | ✓ |
| Integrated recycling system | | ✓ |
| Large-scale deployment | | ✓ |

This distinction is maintained so that the repository accurately represents the current engineering achievement while documenting the broader research direction.

---

# 29. Development Philosophy

AQUA-SWEEP was developed with an emphasis on experimentation rather than assuming that the first design would be optimal.

The development process can be summarised as:

**Observe → Research → Design → Build → Test → Modify → Retest**

The current prototype represents one stage of an ongoing engineering process.

The objective is to use the lessons learned from the physical prototype to simplify, improve and scale future versions.

---

# 30. Project Status

**Current Status: Prototype / Proof of Concept**

The present project documents the physical AQUA-SWEEP prototype, its design methodology, electronics, fabrication process, testing approach and proposed future architecture.

Future versions can expand the platform towards autonomous navigation, environmental sensing, geo-mapping, intelligent waste detection and coordinated multi-robot deployment.

---

# 31. Bill of Materials

The major materials and components used during the development of AQUA-SWEEP include:

- 90 mm PVC pipe
- 40 mm PVC pipe
- Acrylic board
- Sectioned 2-litre plastic bottles
- ESP32 development board
- L298N motor driver
- High-speed propulsion motor
- Main propulsion propeller
- SG90 servo
- IR sensors
- Three 3.7 V 1200 mAh rechargeable cells
- Three 9 V batteries used during experimentation
- Three 9 V battery connectors
- LM2596 voltage regulator
- 4.7 kΩ resistors
- 100 nF capacitors
- 470 µF capacitors
- 400-pin breadboard
- Switch
- M-Seal
- Fevicol
- Feviquick
- Hot glue sticks
- Zip ties
- Paint and brushes
- Connecting wires
- Digital multimeter

A detailed cost breakdown is maintained separately in the project documentation.

---

# 32. Author

**Krishna Deka**

C/O Prof. U.S. Dixit  
IIT Guwahati

---

# AQUA-SWEEP

### From one floating robot to a scalable network of cooperating environmental robots.
