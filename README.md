# 🚦 Practice Tree — Reaction Timer for T-Display S3

A drag racing Christmas tree reaction timer built on the **LILYGO T-Display S3**, using an external LED tree and a single push button. Simulates a full NHRA-style countdown sequence and measures your reaction time to the nearest millisecond.

***

## ✨ Features

- 🌲 **Full Tree mode** — sequential yellow lights, 500ms between each
- ⚡ **Pro Tree mode** — sequential yellow lights, 100ms between each (triple-tap to switch)
- 🎲 **Random stage delay** — 1–7 second random hold after staging before the tree drops
- ⏱️ **Reaction time measurement** — millisecond-accurate, displayed in `.XXX` second format
- 🏆 **Personal best tracking** — best RT persists across all runs in the session
- 📊 **Rolling history** — last 14 reaction times displayed in a 2×7 grid
- 🔵 **Best time highlight** — personal best entry is highlighted in blue in the history grid
- 🔴 **Red light detection** — releasing before green increments the red light counter
- 💡 **Pre-stage debounce** — button must be held 500ms before staging LED fires (prevents accidental triggers)
- 🖥️ **Sprite-buffered display** — flicker-free rendering via LGFX double-buffering

***

## 🖥️ Display Screens

### Idle / History Screen
Shown at startup and after each run is reset.

```
┌─────────────────────┐
│      FULL TREE      │  ← header (green = full, same label for pro)
├──────────┬──────────┤
│ AVERAGE  │  BEST    │
│  .412    │  .308    │  ← .XXX second format, best in blue
├──────────┴──────────┤
│  RECENT RUNS (ms)   │
│  .412  │  .389      │
│  .455  │  .308 ◀best│  ← 2×7 grid, 14 entries max
│  ...   │  ...       │
├─────────────────────┤
│ RED LIGHTS        2 │  ← pinned to bottom
└─────────────────────┘
```

### Staging Screen
Shown once the staging LED fires (after 500ms hold).

```
┌─────────────────────┐
│      FULL TREE      │
│                     │
│      STAGING        │
│  Hold the button... │
└─────────────────────┘
```

### Result Screen — Good Run
```
┌─────────────────────┐
│   REACTION TIME     │
│                     │
│       .412          │
│                     │
│   press for history │
└─────────────────────┘
```

### Result Screen — Red Light
```
┌─────────────────────┐
│                     │
│        RED          │
│       LIGHT         │
│   Press to reset    │
└─────────────────────┘
```

***

## 🕹️ Usage

### Basic Run

1. **Hold** the button — after 500ms the **stage LED** lights up
2. **Keep holding** — after a random 1–7s delay the **yellow lights** sequence fires
3. **Release** the button **as fast as possible** when the **green light** comes on
4. Your reaction time is displayed on screen

### Red Light

- Releasing the button **before green** triggers a red light
- The red light counter increments and the red LED fires
- Press the button to reset to idle

### Mode Switching — Triple Tap

Quickly tap the button **3 times** (each tap < 500ms hold, all within 1500ms window) to toggle between **Full Tree** and **Pro Tree** mode.

| Mode | Yellow Delay | Behavior |
|------|-------------|----------|
| **Full Tree** | 500ms per light | Y1 → Y2 → Y3 → Green, 500ms steps |
| **Pro Tree** | 100ms per light | Y1 → Y2 → Y3 → Green, 100ms steps |

The active mode is shown in the header on every screen.

***

## 🔌 Wiring

### Pin Map

| GPIO | Connected To | Notes |
|------|-------------|-------|
| `1`  | Stage LED (+ resistor) | Staging indicator |
| `2`  | Yellow 1 LED (+ resistor) | First yellow |
| `3`  | Yellow 2 LED (+ resistor) | Second yellow |
| `10` | Yellow 3 LED (+ resistor) | Third yellow |
| `11` | Green LED (+ resistor) | Go light |
| `12` | Red LED (+ resistor) | Red light / foul |
| `13` | Push button (to GND) | Input with `INPUT_PULLUP` |
| `15` | Display power enable | Driven `HIGH` on boot |

### Button Wiring

```
GPIO 13 ──────┬───── Button ───── GND
              │
           (INPUT_PULLUP — internal pull-up enabled)
```

Button reads `LOW` when pressed. No external resistor needed.

### LED Wiring (each LED)

```
GPIO ──── [220–470Ω resistor] ──── LED (+) ──── GND
```

> **Recommended resistor value:** 330Ω for standard 3.3V logic with typical 2V forward-voltage LEDs. Adjust based on your LED's forward voltage and desired brightness.

### Schematic Overview

```
T-Display S3
┌────────────────┐
│ GPIO 1  ───────┼── R ── [STAGE  LED] ── GND
│ GPIO 2  ───────┼── R ── [YELLOW 1  ] ── GND
│ GPIO 3  ───────┼── R ── [YELLOW 2  ] ── GND
│ GPIO 10 ───────┼── R ── [YELLOW 3  ] ── GND
│ GPIO 11 ───────┼── R ── [GREEN  LED] ── GND
│ GPIO 12 ───────┼── R ── [RED    LED] ── GND
│ GPIO 13 ───────┼──────────── [BTN] ──── GND
│ GPIO 15 ───────┼── Display VCC enable
└────────────────┘
```

***

## 🛠️ Build & Flash

### Dependencies

- [Arduino-ESP32](https://github.com/espressif/arduino-esp32) core
- [LovyanGFX](https://github.com/lovyan03/LovyanGFX) (`LGFX_USE_V1`)
- `LGFX_TDisplayS3.h` — board-specific LovyanGFX config for the T-Display S3

### PlatformIO (`platformio.ini` example)

```ini
[env:t-display-s3]
platform  = espressif32
board     = esp32-s3-devkitc-1
framework = arduino
lib_deps  =
    lovyan03/LovyanGFX @ ^1.1.16
build_flags =
    -DLGFX_USE_V1
    -DBOARD_HAS_PSRAM
upload_speed = 921600
monitor_speed = 115200
```

### Arduino IDE

1. Install **ESP32 board package** via Boards Manager
2. Install **LovyanGFX** via Library Manager
3. Select board: **LILYGO T-Display-S3**
4. Upload `main.cpp` 

***

## ⚙️ Configurable Constants

All timing constants are at the top of `main.cpp` for easy tuning:

| Constant | Default | Description |
|----------|---------|-------------|
| `PRE_STAGE_MS` | `500` | Hold time before staging LED fires (ms) |
| `STAGE_DELAY_MS` | `1000–7000` | Random delay before tree drops (ms) |
| `STEP_DELAY_MS` | `500` / `100` | Delay between each yellow light — full / pro tree |
| `TAP_WINDOW_MS` | `1500` | Time window for triple-tap mode switch (ms) |
| `HISTORY_SIZE` | `14` | Number of previous runs stored and displayed |

***

## 📐 State Machine

```
         [hold 500ms]          [random 1-7s]
  IDLE ──────────────▶ STAGING ─────────────▶ Y1_ON
   ▲                      │                     │ 100/500ms
   │                 [release]                  ▼
   │                      │                  Y2_ON
   │                      ▼                     │ 100/500ms
   │                  RED_END                   ▼
   │                      │                  Y3_ON
   │                 [press]                    │ 100/500ms
   │                      │                    ▼
   │                      └──────────────▶ GREEN_ON_HELD
   │                                           │
   │                                    [release after green]
   │                                           │
   └──────────[press]──── GREEN_END ◀──────────┘
                               │
                          [release before green]
                               │
                           RED_END
```

***

## 📄 License

MIT — do whatever you want with it. Build one for your buddy's Christmas present.
