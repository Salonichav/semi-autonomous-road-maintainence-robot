# Semi-Autonomous Road Maintenance Robot

An ESP32-based mobile robotic platform for semi-autonomous road-surface inspection and maintenance.

The firmware implements ultrasonic-based obstacle/pothole detection, automatic motor control, PWM speed regulation, and servo-based inspection positioning. An ESP32-CAM handles real-time visual inspection of the road surface as part of the broader system.

> Developed as a Major Project (2025–2026) by the Dept. of Robotics and Automation Engineering, Angadi Institute of Technology and Management (AITM), Belagavi — under Visvesvaraya Technological University (VTU).

---

## Overview

The robot uses an **ESP32-WROOM-32** as its primary controller to interface with:

- Four DC motors (4-wheel drive)
- L298N dual H-bridge motor driver
- HC-SR04 ultrasonic distance sensor
- MG995 servo motor
- ESP32-CAM module for visual inspection

The robot continuously measures the distance to objects ahead of it. When an obstacle or pothole is detected within a configurable safety threshold, the robot stops and moves the servo to an inspection position. When the path is clear, the robot resumes forward movement and returns the servo to its scanning position.

---

## System Architecture

```
                 ┌──────────────────────┐
                 │        ESP32         │
                 │   Main Controller    │
                 └──────────┬───────────┘
                            │
              ┌─────────────┼─────────────┐
              │             │             │
              ▼             ▼             ▼
       ┌────────────┐ ┌────────────┐ ┌────────────┐
       │ Ultrasonic │ │   L298N    │ │   MG995    │
       │   Sensor   │ │   Driver   │ │   Servo    │
       └────────────┘ └─────┬──────┘ └────────────┘
                             │
                             ▼
                      ┌──────────────┐
                      │  DC Motors   │
                      │  4-Wheel     │
                      │    Drive     │
                      └──────────────┘
                            │
                            ▼
                 ┌──────────────────────┐
                 │      ESP32-CAM       │
                 │  Visual Inspection   │
                 └──────────────────────┘
```

---

## Features

- ESP32-based embedded control
- Four-wheel DC motor drive
- L298N dual H-bridge motor driver
- PWM-based motor speed control (ESP32 LEDC)
- Digital motor direction control
- Ultrasonic obstacle/pothole detection
- Configurable 30 cm obstacle threshold
- Automatic motor stopping on obstacle detection
- MG995 servo-based inspection positioning
- ESP32-CAM integration for visual road-surface inspection
- Serial monitoring of distance readings and robot status

---

## Hardware

| Component          | Purpose                          |
|---------------------|-----------------------------------|
| ESP32-WROOM-32       | Main microcontroller              |
| L298N                | DC motor driver                   |
| 4 × DC Motors         | Robot locomotion                  |
| HC-SR04 Ultrasonic Sensor | Obstacle / pothole detection |
| MG995 Servo Motor     | Controlled camera/sensor actuation |
| ESP32-CAM             | Visual road-surface inspection    |
| 12V Rechargeable Battery | Power source                   |
| Step-Down Converter (3A) | Regulates 12V to 5V / 3.3V     |

---

## Pin Configuration

### Motor Driver (L298N)

| ESP32 Pin | Function             |
|-----------|-----------------------|
| GPIO 13   | Motor A direction – IN1 |
| GPIO 12   | Motor A direction – IN2 |
| GPIO 14   | Motor B direction – IN3 |
| GPIO 27   | Motor B direction – IN4 |
| GPIO 25   | Motor A PWM / ENA      |
| GPIO 26   | Motor B PWM / ENB      |

### Ultrasonic Sensor (HC-SR04)

| ESP32 Pin | Function |
|-----------|----------|
| GPIO 33   | Trigger  |
| GPIO 32   | Echo     |

### Servo

| ESP32 Pin | Function            |
|-----------|-----------------------|
| GPIO 17   | MG995 servo signal    |


---

## Control Logic

The firmware continuously reads the ultrasonic sensor and checks whether an obstacle is within the configured threshold.

```
                 Read distance
                      │
                      ▼
              Distance < 30 cm?
                 /          \
               YES            NO
                │              │
                ▼              ▼
         Stop motors       Move forward
                │              │
                ▼              ▼
          Servo → 90°      Servo → 0°
```

### Obstacle / Pothole Detection

The ultrasonic sensor sends a trigger pulse and measures the duration of the returning echo. Distance is calculated as:

```
Distance (cm) = Echo Duration (µs) × 0.0343 / 2
```

The robot stops when:

```
0 < distance < 30 cm
```

If the sensor times out (no valid echo), the reading is treated as invalid and reported over Serial.

### Motor Control

Motor speed is controlled using the ESP32 LEDC PWM peripheral:

- PWM frequency: **1000 Hz**
- PWM resolution: **8-bit**
- Normal motor speed: **150 / 255**

Motor direction is controlled via the L298N input pins (IN1–IN4).

---

## Software

### Technologies

- C/C++ (Arduino framework)
- ESP32 core (Arduino-ESP32, LEDC PWM API)
- [ESP32Servo](https://github.com/madhephaestus/ESP32Servo) library
- Embedded GPIO control
- Ultrasonic sensing

### Main Firmware Functions

| Function            | Purpose                                         |
|----------------------|--------------------------------------------------|
| `readDistance()`      | Measures ultrasonic distance, returns `-1` on timeout |
| `setMotorSpeed(int)`  | Sets PWM duty cycle (0–255) for both motors      |
| `moveForward(int)`    | Drives both motors forward at a given speed      |
| `stopMotors()`        | Stops both motors and zeroes PWM output          |
| `loop()`               | Runs the obstacle-detection and control logic    |

### Configuration

The following parameters can be adjusted in `src/main.cpp`:

```cpp
const int MOTOR_SPEED = 150;              // 0–255
const float OBSTACLE_DISTANCE = 30.0;     // cm
```

To increase the obstacle detection threshold, for example:

```cpp
const float OBSTACLE_DISTANCE = 40.0;
```

Motor speed can similarly be adjusted anywhere between 0 and 255.

---

## Current Behavior

1. Initialize ESP32 peripherals (motor driver pins, ultrasonic pins, servo, PWM channels).
2. Move the servo to a safe starting position and stop the motors.
3. On every loop iteration:
   - Measure distance using the ultrasonic sensor.
   - If an obstacle is detected within 30 cm:
     - Stop both motors.
     - Move the servo to the inspection position (90°).
   - If the path is clear:
     - Drive forward at the configured speed.
     - Set the servo to its scanning position (0°).
   - Report the distance reading and robot state over Serial.

---

## Getting Started

### Requirements

- [Arduino IDE](https://www.arduino.cc/en/software) or [PlatformIO](https://platformio.org/)
- ESP32 board support package (Arduino-ESP32 core)
- [ESP32Servo](https://github.com/madhephaestus/ESP32Servo) library
- ESP32-CAM board package (for the camera module)

### Build & Upload

1. Clone this repository:
   ```bash
   git clone https://github.com/Salonichav/semi-autonomous-road-maintainence-robot
   ```
2. Open `semi_autonomous_road_maintainence_robot.ino` in the Arduino IDE or PlatformIO.
3. Select the correct ESP32 board and COM port.
4. Install the `ESP32Servo` library via the Library Manager (or `platformio.ini` if using PlatformIO).
5. Build and upload the firmware.
6. Open the Serial Monitor at **115200 baud** to view distance readings and robot status.

---

## Project Structure

```
.
├── semi_autonomous_road_maintainence_robot.ino
├── README.md
└── ...
```

---

## Project Context

This firmware is part of a larger **Semi-Autonomous Road Maintenance Robot** project, providing a mobile platform for road-surface inspection and maintenance. The broader system incorporates an ESP32-CAM for visual inspection, while the ESP32-WROOM-32 handles embedded control, sensing, motor actuation, and obstacle detection.

## Future Improvements

- Autonomous obstacle avoidance
- Left/right directional scanning
- Differential steering
- Improved sensor fusion (IMU, IR)
- ESP32-CAM wireless video streaming
- Remote monitoring and control (Wi-Fi / Bluetooth app)
- AI-based road-surface defect classification
- GPS-based defect location logging
- Full autonomous navigation
- Integration with additional road-maintenance mechanisms (filling/sealing tools)

---
