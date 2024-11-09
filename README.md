# Greenhouse CO₂ Controller System

This project implements a CO₂ controller system for a greenhouse. The controller maintains CO₂ levels based on user settings with safety features and cloud connectivity for remote monitoring and control.

| The Greenhouse System                                                                     |                                                                                           |
| ----------------------------------------------------------------------------------------- | ----------------------------------------------------------------------------------------- |
| ![image](https://github.com/user-attachments/assets/ffefd549-7f6c-4581-93b2-606035b4c1a7) | ![image](https://github.com/user-attachments/assets/7cc72873-d5b0-4670-973a-3d31afb476f3) |
 

### Project Overview

The controller operates based on local and cloud commands:
- **CO₂ Level Control**: Maintains CO₂ levels within the user-set limits (up to 1500 ppm). Settings are stored in EEPROM for persistence.
- **CO₂ Target Setting**: User can set target CO2 level from local UI (Raspberry Pi Pico board), or remotely on ThingSpeak.
- **Safety Feature**: If CO₂ levels exceed 2000 ppm, ventilation automatically increases until the CO₂ level returns to the desired range.
- **Cloud Connectivity**: Connects to ThingSpeak for reporting data and receiving commands using a RESTful API. Network parameters are configurable on the local UI and stored in EEPROM.

### Hardware Components

- **Fan Speed Control**: 
  - Controlled by Produal MIO 12-V via Modbus.
  - **0-10V Output (AO1)**: Adjusts fan speed from 0% (off) to 100% (max).
  - **Digital Input (AI1 Counter)**: Monitors fan rotation pulses. A value of zero after two reads indicates the fan is stopped.

- **Sensors**:
  - **Vaisala GMP252 CO₂ Probe**: Modbus address 240.
  - **Vaisala HMP60 Relative Humidity and Temperature Sensor**: Modbus address 241.
  - **Sensirion SDP610 Pressure Sensor**: I2C address 0x40. Measures pressure difference between room and duct.

- **CO₂ Injection Valve**: injects CO₂ into the greenhouse with a 2-second limit per activation followed by wait period to let CO₂ disperse in the greenhouse and to let the sensor readings to stabilize. 

### Test System

- **Miniature Test System**: 
  - Consists of smaller versions of the components and includes sensor and fan simulations.
  - The miniature system affects controller performance due to its smaller scale but functions identically in software.

### Cloud Connection

Data is sent to ThingSpeak for remote monitoring and control:
- **Data Fields Sent**:
  - CO₂ Level (ppm)
  - Relative Humidity (%)
  - Temperature (°C)
  - Ventilation Fan Speed (%)
  - CO₂ Set Point (ppm)
