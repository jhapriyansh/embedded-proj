# Embedded Device Shell (ESP32)

A **UART-driven, non-blocking embedded shell framework** for the ESP32 that enables **runtime binding, configuration, and execution of hardware devices** through a strict, deterministic command interface.

This project is intentionally designed as a **mini embedded firmware framework**, not a demo.
The focus is on **correctness, safety, and extensibility**, mirroring real embedded systems rather than Arduino-style sketches.

---

## ✨ Core Goals

* Interactive control via **UART (Serial Monitor) only**
* **Strictly non-blocking** execution (`millis()`-based)
* **State-driven hardware execution**
* Explicit **bind / unbind lifecycle**
* Global **GPIO ownership enforcement**
* Deterministic **cooperative scheduler**
* Zero dynamic memory allocation
* Modular, scalable architecture for new devices

---

## 🧠 Design Principles (LOCKED)

These rules are enforced across the codebase and are **non-negotiable**:

* All interaction happens via Serial (UART)
* No `delay()` in runtime logic except for the initial boot delay
* **Configuration ≠ Execution**
* Only **state variables** drive hardware
* Devices must be explicitly **bound** before use
* Scheduler never touches unbound devices
* GPIOs are **globally owned and exclusive**
* No heap usage / dynamic allocation
* Logs capture **intent and lifecycle**, not noise
* One responsibility per module (`.h / .cpp`)

---

## 🏗️ High-Level Architecture

```
[ Serial Console ]
        ↓
[ Token-Based Shell Parser ]
        ↓
[ Command Dispatch ]
        ↓
[ Device Managers ]
        ↓
[ Cooperative Scheduler ]
        ↓
[ Hardware Drivers ]
        ↓
[ GPIO / PWM ]
```

---

## 🔐 GPIO Ownership Model (System-Level)

GPIO ownership is enforced **globally**, not per device.

Rules:

* A GPIO can be claimed by **only one device**
* A device cannot change its pin without `--unbind`
* Unbind **releases** the GPIO
* All devices share the same GPIO manager

This prevents:

* Accidental pin reuse
* Silent hardware conflicts
* Undefined runtime behavior

---

## 🟢 Implemented Subsystems

### 🟢 Shell

* Non-blocking UART input
* **Token-based command parsing** (no `sscanf`)
* Exact flag matching (no prefix ambiguity)
* Deterministic error handling (`ERR` vs `OK`)
* Safe for future command expansion

---

### 🟢 Scheduler

* Cooperative, `millis()`-based
* Fixed task table (no heap)
* Scheduler never touches unbound devices
* Devices control **when** they execute

---

### 🟢 LED Device Subsystem

A fully production-grade reference device.

#### Features

* Multiple LED instances (`id` based, max 4 but easily extensible in the devices_led.cpp file's `#define MAX_LEDS`)
* Explicit bind / unbind lifecycle
* PWM brightness control
* Blink mode (scheduler-driven)
* Morse code output (ITU standard)
* Word-gap (`7T`) support
* Perceptual start/end gaps
* Absolute `state` control

#### Absolute Rule

> `--state=0|1` is the **only execution control**
> All other commands are **configuration only**

#### Commands

```
led <id> -s <pin>
led <id> --state=0|1
led <id> --brightness=0-100
led <id> --blink=0|1
led <id> --morsePulse='sos help'
led <id> --unbind
```

#### Morse Details

* Dot = 1T
* Dash = 3T
* Letter gap = 3T
* Word gap = 7T
* Start gap added for perceptual clarity
* End cooldown before returning to steady ON

---

### 🟢 Logging System

* Fixed-size log buffer
* No heap usage
* Timestamped with `millis()`
* Logs lifecycle and configuration events

Command:

```
log --show
```

Example:

```
[1023] LED bind
[1540] LED unbind
```

---

## 🧪 Hardware Used (Current Stage)

* ESP32 Dev Board (ESP-WROOM-32)
* External LEDs + resistors
* UART via USB (CP2102 / CH340)

No sensors or Wi-Fi involved at this stage.

---

## 🚧 Current Scope (Intentionally Limited)

This stage is focused on **core infrastructure correctness**.

Implemented:

* Shell
* Scheduler
* GPIO ownership
* LED device

Not yet implemented:

* Buzzer
* Servo
* Sensors
* Wi-Fi

This is deliberate to avoid compounding errors before the core is stable.

---

## 🔒 Locked Next Steps

```
1️⃣ Buzzer device (reuse Morse + GPIO manager)
2️⃣ Vibration motor (pattern-based)
3️⃣ Servo (state-driven motion)
4️⃣ PIR / Ultrasonic sensors
5️⃣ OLED status display
6️⃣ On-demand Wi-Fi log upload
```

Each new device must:

* Obey GPIO ownership
* Be non-blocking
* Respect bind / unbind lifecycle
* Integrate with the scheduler

---