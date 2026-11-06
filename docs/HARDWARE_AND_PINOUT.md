# Hardware and pinout register

This document deliberately distinguishes **user-confirmed** connections from **proposed / unverified** connections.

## User-confirmed details

| Module / signal | GPIO / connection | Status |
|---|---:|---|
| Motor driver IN1 | GPIO 32 | GPIO supplied by user |
| Motor driver IN2 | GPIO 33 | GPIO supplied by user |
| Motor driver IN3 | GPIO 26 | GPIO supplied by user |
| Motor driver IN4 | GPIO 14 | GPIO supplied by user |
| IR sensor S1 | GPIO 34 | User-provided sensor diagram |
| IR sensor S2 | GPIO 35 | User-provided sensor diagram |
| IR sensor S3 | GPIO 36 | User-provided sensor diagram |
| IR sensor S4 | GPIO 39 | User-provided sensor diagram |
| IR sensor S5 | GPIO 13 | User-provided sensor diagram |
| IR supply | 3V3 and GND | Previously supplied diagram; check module voltage rating |
| Camera link | Wi-Fi | User-confirmed |
| Display | OLED | User-confirmed display type |
| RFID | RC522 | Module identified in project |
| Environment sensor | DHT11 | Module identified in project |
| Motion sensor | PIR | Module identified in project |
| Distance sensor | Ultrasonic, two-eye type | Module identified in project |

The only motor test explicitly reported as working was a minimal sketch setting IN3=26 HIGH and IN4=14 LOW. This does **not** prove the final left/right motor mapping or direction truth table.

## Proposed connections used as defaults in the example firmware

| Module | Signal | GPIO | Status / action |
|---|---|---:|---|
| RC522 | SDA / SS | 5 | Proposed; verify |
| RC522 | SCK | 18 | Proposed; verify |
| RC522 | MOSI | 23 | Proposed; verify |
| RC522 | MISO | 19 | Proposed; verify |
| RC522 | RST | 4 | Proposed to avoid conflict with IR S5 on GPIO13; physically connect RST to GPIO4 |
| OLED I2C | SDA | 21 | Proposed; verify |
| OLED I2C | SCL | 22 | Proposed; verify |
| DHT11 | DATA | 27 | Proposed; verify |
| PIR | OUT | 25 | Proposed; verify |
| HC-SR04-style sensor | TRIG | 16 | Proposed; verify |
| HC-SR04-style sensor | ECHO | 17 | Proposed; verify |

**Important:** HC-SR04 ECHO is commonly 5V logic. Use a suitable voltage divider / level shifter before connecting it to an ESP32 GPIO.

## Buzzer

The onboard buzzer control GPIO and active polarity have not been confirmed. The main firmware therefore defines `BUZZER_PIN` as `-1` and does not attempt to drive it. Do not guess a GPIO; identify it from the Boodskap HV1 schematic or board documentation first.

## Motor driver

The firmware currently uses the conventional two-input-per-channel model:

- Channel A: IN1 / IN2
- Channel B: IN3 / IN4

The physical wheel grouping and polarity must be calibrated. If the driver board has enable pins or jumpers, confirm their state and wiring. Do not connect or disconnect motors while powered.

## Power

- Do not power motors from the ESP32 3V3 pin.
- Use a motor supply appropriate for the motors and driver.
- Ensure a common ground between the ESP32, motor driver logic, and sensor modules.
- Check the Boodskap board's input limits before supplying power.
