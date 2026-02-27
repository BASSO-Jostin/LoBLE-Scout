# 🛰️ LoBLE-Scout
Low-power GPS tracker combining **LoRa long-range transmission** and **BLE short-range transmission**, built on **Zephyr RTOS**.

The project demonstrates a complete IoT pipeline: from embedded data acquisition to real-time cloud visualization.

---

## 🛠️ Tech Stack

- Zephyr RTOS
- RAK11720 (Apollo3)
- LoRaWAN
- BLE
- MQTT
- Node-RED
- InfluxDB
- Grafana

---

## 🎯 Project Objective

Design and implement a full IoT architecture capable of:

- Acquiring GPS coordinates
- Encoding data efficiently for LoRa transmission
- Transmitting data over **long range (LoRa)** and **over short-range (BLE)**
- Receiving data via MQTT
- Processing with Node-RED
- Visualizing in Grafana

---

## 🏗️ System Architecture


<p align="center"> 
  <img src="https://github.com/BASSO-Jostin/LoBLE-Scout/blob/main/Image/Architecture.png">
</p>

---

## 🔧 Hardware

- Relay (RAK11720 & Module EchoStar)
- GPS module
<div align="center" style="display: flex; justify-content: center; gap: 20px;" >
  <img src="https://github.com/BASSO-Jostin/LoBLE-Scout/blob/main/Image/GPS_Module1.jpg" height="350" />
  <img src="https://github.com/BASSO-Jostin/LoBLE-Scout/blob/main/Image/EchoStar_RAK11720_1.jpg" height="350"/>
</div>


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

## 📶🔵 BLE - short-range transmission

Alerts were received on a smartphone using the nRF Connect application over BLE.

- Short-range transmission
- Distance transmission
- Issuing alerts

<p align="center"> 
  <img src="https://github.com/BASSO-Jostin/LoBLE-Scout/blob/main/Image/BLE_Value.jpg">
</p>

---

## 📡🌍 LoRa - long-range transmission

### 🌐 Node-red Integration

Once received by the Satellite through **LoRa**, data is:

1. Published to an MQTT topic
2. Subscribed by Node-RED
3. Decoded from hex to int32
4. Converted back to floating-point coordinates
5. Stored for visualization in InfluxDB

<p align="center"> 
  <img src="https://github.com/BASSO-Jostin/LoBLE-Scout/blob/main/Image/Node-red.jpg">
</p>

---

### 📊 Visualization

Data is visualized in **Grafana**:

- Direct connection to InfluxDB
- Real-time Position
- Historical position tracking
- GeoMap visualization

<p align="center"> 
  <img src="https://github.com/BASSO-Jostin/LoBLE-Scout/blob/main/Image/GeoMap.jpg">
</p>

---

## 🔋 Power Optimization Strategy

Measured using Otii 3 power analyzer.

- 532 mA during join mode for 1.478 s 
- 63.4 mA when waiting for 4 min
- 308 mA during transmission mode for 914 ms


<p align="center"> 
  <img src="https://github.com/BASSO-Jostin/LoBLE-Scout/blob/main/Image/Consumption.jpg">
</p>

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
```

---

## 🧩 Challenges Faced

- Radio resource conflict between BLE and LoRa
- Channel Busy errors
- RTOS thread synchronization
- Efficient payload encoding under LoRa constraints

---

## 🔮 Future Improvements

- Geofencing
- Mobile application
- Optimisation of energy consumption
 <br>

<p align="center"> 
  <img src="https://github.com/BASSO-Jostin/LoBLE-Scout/blob/main/Image/System.jpg" height="500">
</p>
