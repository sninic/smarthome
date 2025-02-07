# Modular Smart Home System (MSHS)

## Overview
The **Modular Smart Home System (MSHS)** is an open, flexible, and scalable smart home platform designed to integrate various IoT devices seamlessly, regardless of manufacturer. This project addresses the interoperability issues of proprietary smart home ecosystems, reducing electronic waste while empowering users with customization and expansion options.

### Motivation
Modern smart home systems are often expensive, proprietary, and lack interoperability. Consumers are forced to replace incompatible devices, leading to increased electronic waste. The **MSHS** proposes an open-standard, modular system that allows seamless integration of IoT devices, reducing costs and fostering innovation.

## Features
- **Interoperable IoT Devices**: Works with various smart home components, independent of brand.
- **Private Network Connectivity**: Uses a dedicated router to ensure secure and reliable communication.
- **Centralized Control**: Raspberry Pi 4 acts as a hub, connecting devices via Bluetooth Low Energy (BLE).
- **User-Friendly Web Interface**: A simple web app enables users to control and monitor devices.
- **Smart Automation**: Includes an RGB LED module with a photoresistor sensor and an automated blind-raising/lowering system.

## Project Links
- **Demo Video**: [View on Google Drive](https://drive.google.com/file/d/1kg_bsTsK3RIUfnY6Jy4vYGtUB5pycsLE/view?usp=sharing)
- **GitHub Repository**: [Project Code](https://github.com/sninic/smarthome)
- **Reference Guides & Tutorials**:
  - [Arduino Roller Blinds Automation](https://circuitdigest.com/microcontroller-projects/arduino-based-roller-blinds-project-to-automate-and-control-window-curtains-using-nodemcu-and-stepper-motor)
  - [ESP32 Stepper Motor Guide](https://esp32io.com/tutorials/esp32-28byj-48-stepper-motor-uln2003-driver)

---
## Hardware Components
| Component | Purpose | Cost |
|-----------|---------|------|
| **Router** (TP-Link AX1800) | Private network setup | $70 |
| **Raspberry Pi 4** | Central control hub | Owned |
| **ESP32S2 x3** | Microcontrollers for automation | $30 |
| **RGB LED KY-016** | Smart lighting | Owned |
| **Photoresistor** | Ambient light detection | Owned |
| **Stepper Motor 28BYJ-48** | Blinds automation | $15 |
| **ULN2003 Driver Board** | Stepper motor control | Included |
| **12V to 3.3V Buck Converter** | Power regulation | $10 |
| **DC 12V 2A Power Supply** | Powering components | $5 |

---
## System Architecture
### Network & Connectivity
1. **Router**: Configured to create a private subnet.
2. **Hub Device**: Raspberry Pi 4 running a BLE-enabled web server.
3. **IoT Devices**: ESP32 microcontrollers communicate with the hub via BLE.
4. **User Interface**: A web app hosted on the Raspberry Pi allows users to control devices.

**[Insert Network Diagram Here]**

---
## Build Process
### 1. Setting Up the Network
- Configure TP-Link AX1800 to establish a private network.
- Connect Raspberry Pi 4 as the central hub.

### 2. Developing the Lighting Module
- Connect **RGB LED KY-016** to ESP32.
- Implement PWM control for color changes.
- Integrate a **photoresistor sensor** to automate lighting based on ambient brightness.

**[Insert Image of Lighting Module Here]**

### 3. Automating the Blinds
- Attach **Stepper Motor 28BYJ-48** to window blinds.
- Use **ULN2003 Driver Board** for motor control.
- Modify the system for non-invasive installation.

**[Insert Image of Blinds Module Here]**

### 4. Web App & BLE Integration
- Set up a Flask-based web server on the Raspberry Pi.
- Develop a simple UI for device control.
- Implement BLE communication between ESP32 and the Raspberry Pi.

**[Insert Web App Screenshot Here]**

---
## Challenges & Future Improvements
### Key Challenges
- **Limited Network Access**: Required setting up a private network to bypass apartment network restrictions.
- **Motor Strength for Blinds**: The stepper motor struggled to move heavier blinds.
- **Non-Invasive Installation**: Difficulties securing the blinds module without drilling.

### Potential Enhancements
- Implement device pairing and persistent connection tracking.
- Expand to other smart home modules like temperature sensors.
- Improve mounting solutions for blind automation.
- Develop a mobile app for additional control options.

---
## Conclusion
The **MSHS** demonstrates a flexible, scalable, and user-friendly approach to smart home automation. While there are areas for improvement, the project successfully showcases the potential of open-source smart home systems.

**For more details, check out the code in the repo**

