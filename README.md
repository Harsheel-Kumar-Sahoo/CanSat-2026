# CanSat Flight Software: The Electrophiles

ESP32-based CanSat firmware that reads altitude, pressure, temperature, humidity, acceleration and angular rate, and transmits them every second as a telemetry packet over a 433 MHz LoRa link to a ground station.

## Features

- Altitude, pressure and temperature from a **BMP180**
- 3-axis acceleration and 3-axis gyroscope data from an **MPU6050**
- Temperature and humidity from a **DHT22**
- Live telemetry over **LoRa (433 MHz)** at 1 Hz
- Status LED: blink codes for sensor failures, 3 blinks when ready, short flash on each packet
- Serial debug output (115200 baud) showing every packet sent

## Hardware

| Component | Model | Purpose |
|-----------|-------|---------|
| Microcontroller | ESP32 | Main controller |
| Pressure / altitude sensor | BMP180 | Pressure, altitude |
| IMU | MPU6050 | Acceleration, gyroscope |
| Temp / humidity sensor | DHT22 | Temperature, humidity |
| Radio | LoRa module (SX127x, 433 MHz) | Telemetry downlink |
| Indicator | LED | Status |

## Pin Connections

| Device | Signal | ESP32 Pin |
|--------|--------|-----------|
| I2C bus (BMP180, MPU6050) | SDA | GPIO 21 |
| I2C bus (BMP180, MPU6050) | SCL | GPIO 22 |
| DHT22 | Data | GPIO 4 |
| LoRa | SCK | GPIO 18 |
| LoRa | MISO | GPIO 19 |
| LoRa | MOSI | GPIO 23 |
| LoRa | NSS / CS | GPIO 5 |
| LoRa | RST | GPIO 14 |
| LoRa | DIO0 | GPIO 2 |
| Status LED | Anode | GPIO 13 |

## LoRa Settings

| Parameter | Value |
|-----------|-------|
| Frequency | 433 MHz |
| Spreading factor | 7 |
| Bandwidth | 125 kHz |
| Coding rate | 4/5 |
| Sync word | 0xA5 |
| CRC | Enabled |
| TX power | 17 dBm |

The receiver must use the **same** settings or it will not receive packets.

## Libraries Required

Install through the Arduino IDE Library Manager:

- **LoRa** by Sandeep Mistry
- **Adafruit MPU6050**
- **Adafruit BMP085 Library** (also works for BMP180)
- **Adafruit Unified Sensor**
- **DHT sensor library** by Adafruit

`Wire` and `SPI` are included with the ESP32 board package.

## Setup & Upload

1. Install the [Arduino IDE](https://www.arduino.cc/en/software).
2. Add ESP32 board support (Boards Manager, search "esp32" by Espressif).
3. Install the libraries listed above.
4. Open the `.ino` file (the sketch folder name must match the file name).
5. Select your ESP32 board and COM port.
6. Upload, then open the Serial Monitor at **115200 baud**.

## Status LED Codes

| Pattern | Meaning |
|---------|---------|
| Fast blink (200 ms), repeating | BMP180 not detected |
| Slow blink (400 ms), repeating | MPU6050 not detected |
| 3 blinks at startup | All initialised, transmitter ready |
| Short flash (50 ms) | Packet transmitted |

## Telemetry Packet Format

One packet is sent every second as a text string:

```
TeamName; P-<packet no>; Ti-<hh:mm:ss:ms>; A-<alt>; Pr-<pressure>; T-<temp>; Ro-<gx>; Pi-<gy>; Y-<gz>; AX-<ax>; AY-<ay>; AZ-<az>;
```

Example:

```
The Electrophiles; P-001; Ti-00:00:01:002;A-312.4; Pr-97650.12; T-28.3;Ro-0.1; Pi--0.0; Y-0.2;AX-0.05; AY-0.12; AZ-9.78;
```

| Field | Meaning | Unit |
|-------|---------|------|
| Team name | Team identifier | n/a |
| P | Packet counter | n/a |
| Ti | Time since boot (hh:mm:ss:ms) | n/a |
| A | Altitude (BMP180, sea level reference 1013.25 hPa) | m |
| Pr | Pressure | Pa |
| T | Temperature (DHT22) | °C |
| Ro / Pi / Y | Gyro X / Y / Z (roll, pitch, yaw rate) | rad/s |
| AX / AY / AZ | Acceleration X / Y / Z | m/s² |

If the DHT22 read fails, the temperature value is sent as `-999`.

## Project Structure

```
.
├── <sketch-name>.ino   # Main firmware
├── docs/               # Wiring diagram, photos
└── README.md
```

## Known Limitations / Future Work

- No onboard SD card logging (telemetry only)
- No GPS
- Altitude uses a fixed sea-level pressure of 1013.25 hPa; calibrate to launch-site pressure for better accuracy
- Receiver code is not included in this repository

## Team

**The Electrophiles**

- Name: role
- Name: role

## License

MIT (add a `LICENSE` file)
