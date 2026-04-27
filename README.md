# 🚦 Practice Tree — Reaction Timer for T-Display S3

A drag racing Christmas tree reaction timer built on the **LILYGO T-Display S3**, using an external LED tree and a single push button. Simulates a full NHRA-style countdown sequence and measures your reaction time to the nearest millisecond.

---

## ✨ Features

- 🌲 **Full Tree mode** — sequential yellow lights, 500ms between each
- ⚡ **Pro Tree mode** — sequential yellow lights, 100ms between each (triple-tap to switch)
- 🎲 **Random stage delay** — 1–7 second random hold after staging before the tree drops
- ⏱️ **Reaction time measurement** — millisecond-accurate, displayed in `.XXX` second format
- 🏆 **Personal best tracking** — best RT persists across all runs in the session, highlighted in blue
- 📊 **Rolling history** — last 12 reaction times displayed in a 2×6 grid
- 🔴 **Red light detection** — releasing before green increments the red light counter
- 💡 **Pre-stage debounce** — button must be held 500ms before staging LED fires (prevents accidental triggers)
- 🖥️ **Sprite-buffered display** — flicker-free rendering via LGFX double-buffering

---

## 🛠️ Bill of Materials

### Electronics

| Qty | Component | Specs | Notes |
|-----|-----------|-------|-------|
| 1 | LILYGO T-Display S3 | ESP32-S3, 1.9" ST7789 LCD | Main controller with integrated display |
| 1 | Perfboard | 5×7cm | LED tree assembly |
| 7 | Resistors | 680Ω, 1/4W | Current limiting — one per LED |
| 5 | Yellow LEDs | 5mm | Stage + tree countdown sequence |
| 1 | Green LED | 5mm | Go light |
| 1 | Red LED | 5mm | Red light / foul indicator |
| 1 | Push button | Momentary SPST | Trigger, wired to GPIO13 |
| 1 | Power switch | DPDT latching (6-pin) | Reset bypass for hard power-off |
| 1 | LiPo battery | 3.7V 2000mAh with JST connector | Portable power source |

### Hardware

| Qty | Size | Purpose |
|-----|------|---------|
| 4 | 2mm × 8mm machine screws | Perfboard / component mounting |
| 4 | 2mm × 12mm machine screws | Enclosure assembly |

---

## 🔌 Wiring

### Pin Map

| GPIO | Connected To | Notes |
|------|-------------|-------|
| `1`  | Stage LED + 680Ω resistor | Staging indicator |
| `2`  | Yellow 1 LED + 680Ω resistor | First yellow |
| `3`  | Yellow 2 LED + 680Ω resistor | Second yellow |
| `10` | Yellow 3 LED + 680Ω resistor | Third yellow |
| `11` | Green LED + 680Ω resistor | Go light |
| `12` | Red LED + 680Ω resistor | Red light / foul |
| `13` | Push button (to GND) | `INPUT_PULLUP` — reads LOW when pressed |
| `15` | Display power enable | Driven `HIGH` on boot to power the display |

### Button Wiring

The button uses the ESP32's internal pull-up — no external resistor needed:

```
GPIO 13 ───────────── Button ───────────── GND
              (INPUT_PULLUP)
```

### LED Wiring (per LED)

```
GPIO ──── [680Ω] ──── LED (+) ──── GND
```

### Power Switch — Reset Bypass

The DPDT latching switch forces an off state by shorting the RST signal pad on the reset button to GND:

```
RST signal pad ───── [Switch] ───── GND
```

When latched closed, the RST line is held LOW and the board stays in reset (off). Open the switch and the board boots normally.

**To wire:**
1. Solder a wire to the RST signal pad on the T-Display S3 reset button
2. Solder a wire to any GND pad or pin
3. Run both to your DPDT latching switch

### Schematic Overview

```
T-Display S3
┌────────────────┐
│ GPIO 1  ───────┼── 680Ω ── [STAGE  LED] ── GND
│ GPIO 2  ───────┼── 680Ω ── [YELLOW 1  ] ── GND
│ GPIO 3  ───────┼── 680Ω ── [YELLOW 2  ] ── GND
│ GPIO 10 ───────┼── 680Ω ── [YELLOW 3  ] ── GND
│ GPIO 11 ───────┼── 680Ω ── [GREEN  LED] ── GND
│ GPIO 12 ───────┼── 680Ω ── [RED    LED] ── GND
│ GPIO 13 ───────┼──────────── [BTN] ──── GND
│ GPIO 15 ───────┼── Display power enable
│ BAT+    ───────┼── LiPo (+) 3.7V JST
│ GND     ───────┼── LiPo (-)
│ RST Pad ───────┼── [Power Switch] ──── GND
└────────────────┘
```

---

## 🖥️ Display Screens

### Idle / History Screen

```
┌─────────────────────┐
│      FULL TREE      │  ← header (switches to PRO TREE)
├──────────┬──────────┤
│ AVERAGE  │  BEST    │
│  .412    │  .308    │  ← .XXX format; best in blue
├──────────┴──────────┤
│  RECENT RUNS (ms)   │
│  .412  │  .389      │
│  .455  │  .308      │  ← 2×6 grid, 12 entries max
│  ...   │  ...       │  ← best time highlighted in blue
├─────────────────────┤
│ RED LIGHTS        2 │  ← pinned to bottom
└─────────────────────┘
```

### Staging Screen

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

---

## 🕹️ Usage

### Basic Run

1. **Hold** the button — after 500ms the **stage LED** lights up
2. **Keep holding** — after a random 1–7s delay the yellow sequence fires
3. **Release** as fast as possible when the **green light** comes on
4. Your reaction time is displayed in `.XXX` format
5. **Press** again to return to the history screen

### Red Light

- Releasing **before green** fires the red LED and increments the counter
- Press the button to reset to idle

### Mode Switching — Triple Tap

Tap the button **3 times** quickly (each tap < 500ms hold, all 3 within a 1500ms window) to toggle between Full Tree and Pro Tree.

| Mode | Yellow Behavior | Timing |
|------|----------------|--------|
| **Full Tree**| 500ms per step |
| **Pro Tree** | 100ms to green |

The active mode is shown in the header on every screen.

---

## 🛠️ Build & Flash

### Project Structure

```
practiceTree/
├── platformio.ini
├── src/
│   ├── main.cpp
│   └── LGFX_TDisplayS3.h
├── .gitignore
└── .gitattributes
```

### platformio.ini

```ini
[env:esp32s3]
platform      = espressif32
board         = esp32-s3-devkitc-1
framework     = arduino
monitor_speed = 115200
upload_speed  = 921600
lib_deps      =
    lovyanGFX
build_flags   =
    -D LGFX_USE_V1
```

### Arduino IDE

1. Install **ESP32 board package** via Boards Manager
2. Install **LovyanGFX** via Library Manager
3. Select board: **LILYGO T-Display-S3**
4. Copy both `src/main.cpp` and `src/LGFX_TDisplayS3.h` into the same sketch folder
5. Upload

---

## ⚙️ Configurable Constants

All tunable values are at the top of `src/main.cpp`:

| Constant | Default | Description |
|----------|---------|-------------|
| `PRE_STAGE_MS` | `500` | Hold time before staging LED fires (ms) |
| `STAGE_DELAY_MS` | `1000–7000` | Randomized delay before tree drops (ms) |
| `STEP_DELAY_MS` | `500` / `100` | Full tree step delay / Pro tree delay to green (ms) |
| `TAP_WINDOW_MS` | `1500` | Time window for triple-tap mode switch (ms) |
| `HISTORY_SIZE` | `12` | Number of previous runs stored and displayed |

---

## 📐 State Machine

```
         [hold 500ms]           [random 1–7s]
  IDLE ──────────────▶ PRE_STAGE ────────────▶ STAGING
   ▲                      │                      │
   │                 [release]             [delay elapsed]
   │                 (tap counted)               │
   │                      ▼              ┌───────┴────────┐
   │                    IDLE         Full Tree        Pro Tree
   │                                    │                │
   │                                  Y1_ON         PRO_YELLOW
   │                                    │ 500ms      (all 3 on)
   │                                  Y2_ON              │ 100ms
   │                                    │ 500ms           │
   │                                  Y3_ON               │
   │                                    │ 500ms           │
   │                                    └────────┬────────┘
   │                                             ▼
   │                                       GREEN_ON_HELD
   │                                             │
   │                    [release before green]   │   [release after green]
   │                             │               │
   │                          RED_END        GREEN_END
   │                             │               │
   └──────────[press]────────────┴───────────────┘
```

---

## 📄 License

MIT — do whatever you want with it. Build one for your buddy's Christmas present.
