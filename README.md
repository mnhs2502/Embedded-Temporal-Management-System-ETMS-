# Embedded Temporal Management System (ETMS)

An industrial-grade, state-machine-driven productivity controller developed for the **ATmega328P (Arduino Nano)** platform. This project demonstrates the complete engineering lifecycle, from firmware architecture and virtual circuit verification in **Proteus VSM** to a verified **physical hardware prototype**.

## 🛠️ Project Overview
The ETMS is a specialized embedded solution designed to automate productivity cycles. By utilizing the **I2C communication protocol** and a non-blocking timing architecture, the system provides high responsiveness and precision for time-critical tasks.

### 🚀 Key Technical Features:
* **Preset Operational Profiles (Dual-Mode):** Integrated support for industry-standard intervals:
    * **Profile A:** 25-minute Work / 5-minute Break.
    * **Profile B:** 50-minute Work / 10-minute Break.
* **Finite State Machine (FSM):** Robust logic handling transitions between `WORK`, `BREAK`, `PAUSE`, and `ALARM` states.
* **Non-Blocking Timing:** Implemented using `millis()` logic to ensure continuous monitoring of user inputs (button interrupts) without processor stalls.
* **I2C Bus Integration:** Drives a 16x2 LCD via the **PCF8574** I/O expander, optimizing the hardware footprint by reducing 6-pin parallel wiring to a 2-wire serial bus (**SDA/SCL**).
* **EEPROM Persistence:** Automatic caching of session progress to non-volatile memory to prevent data loss during power interruptions.

---

## 🏗️ System Architecture

### Hardware Configuration
| Signal Name | MCU Pin | Peripheral | Purpose |
| :--- | :--- | :--- | :--- |
| **MAIN_CTRL** | D2 | Pulse Button | Start / Pause / Reset |
| **MODE_SEL** | D3 | Pulse Button | **Toggle between 25/5 and 50/10 Profiles** |
| **I2C_SDA** | A4 | PCF8574    | Data Line (4.7kΩ Pull-up) |
| **I2C_SCL** | A5 | PCF8574    | Clock Line (4.7kΩ Pull-up) |
| **IND_WORK** | D10| LED (Red)  | Active Work Phase Indicator |
| **IND_BREAK** | D11| LED (Green)| Active Break Phase Indicator |

### Firmware Logic
The firmware utilizes a non-polling approach for timing to allow for:
1.  **Real-Time Parameter Switching:** Dynamically reassigning timer variables when the user toggles between the 25/5 and 50/10 presets.
2.  **Software-Based Debouncing:** Filtering mechanical noise from tactile switches to ensure reliable state transitions in the physical prototype.

---

## 🔬 Development Lifecycle

### Phase 1: Virtual Prototyping (Proteus)
Before physical assembly, the circuit was modeled and stress-tested in **Proteus 8.9**. This phase was critical for:
* Resolving `DSIM.DLL` access violations through optimized firmware linking.
* Verifying I2C pull-up resistor requirements (4.7kΩ) for stable communication.
* Validating the interrupt logic for button interactions across different timing profiles.

### Phase 2: Physical Implementation
The system was transitioned from simulation to a physical breadboard prototype to validate real-world performance.
* **Hardware Validation:** Confirmed that the I2C backpack and LCD operated correctly under physical voltage conditions.
* **Tactility Testing:** Verified that the firmware successfully handled physical switch "bounce" during rapid profile switching.

---

## 🚀 Deployment Instructions

1.  **Simulation:**
    * Open `/Hardware/ETMS_Hardware_Sim_v1.pdsprj` in Proteus.
    * Link the compiled `.hex` file to the ATmega328P component.
    * Run the simulation to observe state and profile transitions.

2.  **Physical Hardware:**
    * Flash the `ETMS_v1.ino` source code using the Arduino IDE.
    * Connect the I2C LCD backpack to Pins A4 (SDA) and A5 (SCL).
    * *Note: Ensure the I2C address in the code matches your physical module (typically 0x27 or 0x3F).*

---

## 📁 Repository Structure
```text
├── Firmware/             # C++ source code and compiled binaries (.hex)
├── Hardware/             # Proteus project files and schematic PDFs
├── Media/                # Simulation screenshots and prototype photos
└── README.md             # Technical documentation
