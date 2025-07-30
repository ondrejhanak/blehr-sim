# BLE Heart Rate Monitor Simulator (ESP32)

An ESP32-based BLE peripheral that simulates a Bluetooth Heart Rate Monitor. Useful for testing BLE clients without needing a real chest strap.

Created as a testing tool for my demo [iOS app](https://github.com/ondrejhanak/blehr-ios).

## Features

- Advertises standard **Heart Rate Service** (`UUID 0x180D`)  
- Implements **Heart Rate Measurement** characteristic (`UUID 0x2A37`)  
- Implements **Client Characteristic Configuration Descriptor** (`UUID 0x2902`) enabling notifications
- Sends both **8-bit** and **16-bit** (above 255) BPM values
- Simulates a natural sine-wave heart rhythm with randomized spikes  
- Written in C++
- Uses the **NimBLE-Arduino** BLE library

## Requirements

- Any **ESP32 development board with Bluetooth**  
- **No external wiring or sensors** required, this is a pure software simulator
- **VS Code** with **PlatformIO** plugin
