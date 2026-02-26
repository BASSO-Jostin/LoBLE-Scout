# 🛰️ LoBLE-Scout
Low-power GPS tracker combining **LoRa long-range transmission** and **BLE short-range transmission**, built on **Zephyr RTOS**.

The project demonstrates a complete IoT pipeline: from embedded data acquisition to real-time cloud visualization.

---

## 🎯 Project Objective

Design and implement a full IoT architecture capable of:

- Acquiring GPS coordinates
- Encoding data efficiently for LoRa transmission
- Transmitting data over long range (LoRa) and over short-range (BLE)
- Receiving data via MQTT
- Processing with Node-RED
- Visualizing in Grafana

---

## 🏗️ System Architecture





---

## 🔧 Hardware

- Relay (RAK11720 & Module EchoStar)
- GPS module


---

## 🧠 Software Architecture

Built with **Zephyr RTOS** using multi-threading:

- GPS Thread → Position acquisition
- LoRa Thread → Data transmission
- BLE Thread → calculation and transmission of the distance traveled
- Kernel synchronization using mutex/semaphores

Zephyr ensures:
- Deterministic scheduling
- Safe radio resource sharing
- Low power management

---

## 📡 Payload Format

To minimize LoRa payload size, GPS coordinates are encoded as signed 32-bit integers:

Payload structure:

| Bytes | Content      |
|-------|-------------|
| 0–3   | Longitude   |
| 4–7   | Latitude    |

Total: **8 bytes**

---

## 🌐 Node-red Integration

Once received by the LoRa gateway, data is:

1. Published to an MQTT topic
2. Subscribed by Node-RED
3. Decoded from hex to int32
4. Converted back to floating-point coordinates
5. Stored for visualization

---

## 📊 Visualization

Data is visualized in **Grafana**:

- Real-time latitude & longitude
- Historical position tracking
- Time-series visualization

---

## 🔋 Power Optimization Strategy

- Periodic transmission (configurable interval)
- Low-power sleep modes
- Efficient payload encoding
- Radio resource arbitration between BLE and LoRa

---

## 🚀 Build Instructions

### Requirements

- Zephyr SDK
- west tool
- Proper board configuration

### Build & Flash

```bash
To build : west build -b rak11720 .
To flash : west flash



## 🧩 Challenges Faced

- Radio resource conflict between BLE and LoRa
- Channel Busy errors
- RTOS thread synchronization
- Efficient payload encoding under LoRa constraints

## 🔮 Future Improvements

- Geofencing
- Adaptive transmission rate
- Mobile application
- Payload compression
- OTA firmware updates
