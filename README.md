# Embedded Temporal Management System (ETMS)

An industrial-grade, state-machine-driven productivity controller developed for the **ATmega328P (Arduino Nano)** platform. This project demonstrates the complete engineering lifecycle, from firmware architecture and virtual circuit verification in **Proteus VSM** to a verified **physical hardware prototype**.

## 🛠️ Project Overview
The ETMS is designed to automate work-rest cycles (Pomodoro technique) using a non-blocking firmware architecture. By utilizing the **I2C communication protocol**, the system minimizes GPIO overhead, allowing for future expansion of sensors or actuators.

### Key Technical Features:
* **Finite State Machine (FSM):** Robust logic handling transitions between `WORK`, `BREAK`, `PAUSE`, and `ALARM` states.
* **Non-Blocking Timing:** Implemented using `millis()` logic to ensure high responsiveness for user inputs and background tasks without processor stalls.
* **I2C Bus Integration:** Drives a 16x2 LCD via the **PCF8574** I/O expander, reducing 6-pin parallel wiring to a 2-wire serial bus (**SDA/SCL**).
* **EEPROM Data Persistence:** Automatic caching of cycle progress to non-volatile memory to prevent session loss during power interruptions.
* **Hardware Verification:** Full schematic simulation in Proteus to pre-verify signal integrity, I2C addressing (**0x27**), and bus contention issues.

---

## 🏗️ System Architecture

### Hardware Configuration
| Signal Name | MCU Pin | Peripheral | Purpose |
| :--- | :--- | :--- | :--- |
| **MAIN_CTRL** | D2 | Pulse Button | Start / Pause / Long Press Reset |
| **MODE_SEL** | D3 | Pulse Button | Toggle Work/Break Durations |
| **I2C_SDA** | A4 | PCF8574 | Data Line (4.7kΩ Pull-up) |
| **I2C_SCL** | A5 | PCF8574 | Clock Line (4.7kΩ Pull-up) |
| **IND_WORK** | D10 | LED (Red) | Active Work Phase Indicator |
| **IND_BREAK** | D11 | LED (Green) | Active Break Phase Indicator |

### Firmware Logic
The core logic resides in a state-independent loop that monitors system time without halting the processor. This enables:
1.  **Software-Based Debouncing:** Filtering mechanical noise from physical tactile switches for reliable triggering.
2.  **Efficient Bus Management:** Display updates are triggered only on state changes to minimize I2C bus traffic.

---

## 🔬 Development Lifecycle

### Phase 1: Virtual Prototyping (Proteus)
Before physical assembly, the circuit was modeled and stress-tested in **Proteus 8.9**. This phase was critical for:
* Resolving `DSIM.DLL` access violations through optimized firmware linking.
* Verifying I2C pull-up resistor requirements (4.7kΩ) for stable communication.
* Validating the interrupt logic for button interactions.

### Phase 2: Physical Implementation
The system was transitioned from simulation to a physical breadboard prototype to validate real-world performance.
* **Hardware Validation:** Verified that the I2C backpack and LCD operated correctly under physical voltage conditions.
* **Tactility Testing:** Confirmed the software's ability to handle physical switch "bounce" during rapid state transitions.

---

## 🚀 Deployment Instructions

1.  **Simulation:**
    * Open `/Hardware/ETMS_Schematic.pdsprj` in Proteus.
    * Link the `.hex` file (found in `/Firmware/Output/`) to the ATmega328P component.
    * Run the simulation to observe state transitions.

2.  **Physical Hardware:**
    * Flash the `.ino` source code using the Arduino IDE.
    * Connect the I2C LCD backpack to Pins A4 (SDA) and A5 (SCL).
    * *Note: Use internal pull-ups or external 4.7kΩ resistors on the I2C lines.*

---

## 📁 Repository Structure
```text
├── Firmware/             # C++ source code and compiled binaries
├── Hardware/             # Proteus project files and schematic PDFs
├── Media/                # Simulation screenshots and prototype photos
└── README.md             # Technical documentation
