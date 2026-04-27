🚦 Practice Tree — Reaction Timer for T-Display S3
A drag racing Christmas tree reaction timer built on the LILYGO T-Display S3, using an external LED tree and a single push button. Simulates a full NHRA-style countdown sequence and measures your reaction time to the nearest millisecond.

✨ Features
🌲 Full Tree mode — sequential yellow lights, 500ms between each

⚡ Pro Tree mode — all three yellows simultaneously, instant-on (triple-tap to switch)

🎲 Random stage delay — 1–7 second random hold after staging before the tree drops

⏱️ Reaction time measurement — millisecond-accurate, displayed in .XXXX second format

🏆 Personal best tracking — best RT persists across all runs in the session and highlighted in blue

📊 Rolling history — last 14 reaction times displayed in a 2×7 grid

🔴 Red light detection — releasing before green increments the red light counter

💡 Pre-stage debounce — button must be held 500ms before staging LED fires (prevents accidental triggers)

🖥️ Sprite-buffered display — flicker-free rendering via LGFX double-buffering

📱 Portrait orientation — screen rotated 90° clockwise for optimal viewing in enclosure

🛠️ Bill of Materials
Electronics
Qty	Component	Specs	Notes
1	LILYGO T-Display S3	ESP32-S3, 1.9" ST7789 LCD	Main controller with integrated display
1	Perfboard	5×7cm	For LED tree assembly
7	Resistors	680Ω, 1/4W	Current limiting for LEDs (1 per LED)
5	Yellow LEDs	5mm	Tree countdown sequence
1	Green LED	5mm	Go light
1	Red LED	5mm	Red light / foul indicator
1	Push button	Momentary SPST	Trigger button, wired to GPIO13
1	Power switch	DPDT latching (6-pin)	Reset bypass for hard power-off
1	LiPo battery	3.7V 2000mAh with JST connector	Portable power source
Hardware
Qty	Component	Size	Purpose
4	Machine screws	2mm × 8mm	Perfboard/component mounting
4	Machine screws	2mm × 12mm	Enclosure assembly
Wiring
22-24 AWG hookup wire (various colors recommended for organization)

Heat shrink tubing (optional, for clean solder joints)

JST connector (pre-attached to LiPo battery)

🔌 Wiring
Pin Map
GPIO	Connected To	Notes
1	Stage LED (+ 680Ω resistor)	Staging indicator
2	Yellow 1 LED (+ 680Ω resistor)	First yellow
3	Yellow 2 LED (+ 680Ω resistor)	Second yellow
10	Yellow 3 LED (+ 680Ω resistor)	Third yellow
11	Green LED (+ 680Ω resistor)	Go light
12	Red LED (+ 680Ω resistor)	Red light / foul
13	Push button (to GND)	Input with INPUT_PULLUP
Button Wiring
The button uses the ESP32's internal pull-up resistor, requiring no external resistor:

text
GPIO 13 ──────┬───── Button ───── GND
              │
           (INPUT_PULLUP — internal pull-up enabled)
Button reads LOW when pressed.

LED Wiring (each LED)
Each LED requires a 680Ω current-limiting resistor:

text
GPIO ──── [680Ω resistor] ──── LED (+) ──── GND
The 680Ω value is specifically chosen for 3.3V logic with standard 2V forward-voltage LEDs.

Power Switch Wiring — Reset Button Bypass
The DPDT latching switch forces an "off" state by bypassing the reset button signal pad to ground:

text
RST Pad ───── [Switch] ───── GND
     (on reset button)
When the switch is latched closed, it grounds the RST line, holding the board in a permanent reset (OFF) state. When you unlatch the switch, the connection opens and the board boots normally.

To wire this:

Solder one wire to the RST signal pad on the reset button

Solder another wire to any GND pad or pin

Connect both wires to your DPDT latching switch

Schematic Overview
text
T-Display S3
┌────────────────┐
│ GPIO 1  ───────┼── 680Ω ── [STAGE  LED] ── GND
│ GPIO 2  ───────┼── 680Ω ── [YELLOW 1  ] ── GND
│ GPIO 3  ───────┼── 680Ω ── [YELLOW 2  ] ── GND
│ GPIO 10 ───────┼── 680Ω ── [YELLOW 3  ] ── GND
│ GPIO 11 ───────┼── 680Ω ── [GREEN  LED] ── GND
│ GPIO 12 ───────┼── 680Ω ── [RED    LED] ── GND
│ GPIO 13 ───────┼──────────── [BTN] ──── GND
│                │
│ BAT+    ───────┼── LiPo (+) 3.7V
│ GND     ───────┼── LiPo (-)
│                │
│ RST Pad ───────┼── [Power Switch] ── GND
└────────────────┘
🖥️ Display Screens
Idle / History Screen
Shown at startup and after each run is reset.

text
┌─────────────────────┐
│      FULL TREE      │  ← header (changes with mode)
├──────────┬──────────┤
│ AVERAGE  │  BEST    │
│  .4120   │  .3080   │  ← .XXXX second format, best in blue
├──────────┴──────────┤
│  RECENT RUNS (ms)   │
│  .4120 │  .3890     │
│  .4550 │  .3080 ◀   │  ← 2×7 grid, 14 entries max
│  ...   │  ...       │  ← best highlighted in blue
├─────────────────────┤
│ RED LIGHTS        2 │  ← pinned to bottom
└─────────────────────┘
Staging Screen
Shown once the staging LED fires (after 500ms hold).

text
┌─────────────────────┐
│      FULL TREE      │
│                     │
│      STAGING        │
│  Hold the button... │
└─────────────────────┘
Result Screen — Good Run
text
┌─────────────────────┐
│   REACTION TIME     │
│                     │
│       .4120         │
│                     │
│   press for history │
└─────────────────────┘
Result Screen — Red Light
text
┌─────────────────────┐
│                     │
│        RED          │
│       LIGHT         │
│   Press to reset    │
└─────────────────────┘
🕹️ Usage
Basic Run
Hold the button — after 500ms the stage LED lights up

Keep holding — after a random 1–7s delay the yellow lights sequence fires

Release the button as fast as possible when the green light comes on

Your reaction time is displayed on screen in .XXXX format

Press the button again to return to the history screen

Red Light
Releasing the button before green triggers a red light

The red light counter increments and the red LED fires

Press the button to reset to idle

Mode Switching — Triple Tap
Quickly tap the button 3 times (each tap < 500ms hold, all within 1500ms window) to toggle between Full Tree and Pro Tree mode.

Mode	Yellow Behavior	Timing
Full Tree	Sequential Y1 → Y2 → Y3 → Green	500ms per light
Pro Tree	All three yellows simultaneously → Green	100ms to green
The active mode is shown in the header on every screen.

🛠️ Build & Flash
Project Structure
Your PlatformIO project should follow this layout:

text
practiceTree/
├── platformio.ini
├── src/
│   └── main.cpp
├── include/
│   └── LGFX_TDisplayS3.h
├── lib/
└── test/
Dependencies
Arduino-ESP32 core

LovyanGFX (LGFX_USE_V1)

LGFX_TDisplayS3.h — board-specific LovyanGFX config for the T-Display S3

PlatformIO (platformio.ini)
text
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
Arduino IDE
Install ESP32 board package via Boards Manager

Install LovyanGFX via Library Manager

Select board: LILYGO T-Display-S3

Upload main.cpp

⚙️ Configurable Constants
All timing constants are at the top of main.cpp for easy tuning:

Constant	Default	Description
PRE_STAGE_MS	500	Hold time before staging LED fires (ms)
STAGE_DELAY_MS	1000–7000	Random delay before tree drops (ms)
STEP_DELAY_MS	500 / 100	Delay between each yellow light — full / pro tree
TAP_WINDOW_MS	1500	Time window for triple-tap mode switch (ms)
HISTORY_SIZE	14	Number of previous runs stored and displayed
📐 State Machine
The reaction timer operates as a finite state machine:

text
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
🔧 Assembly Notes
Perfboard Layout
Mount all 7 LEDs on the 5×7cm perfboard in a vertical tree arrangement:

Top: 1 × Stage LED (any color, typically blue or white)

Middle: 3 × Yellow LEDs (sequential)

Bottom: 1 × Green LED, 1 × Red LED (side by side)

Each LED shares a common ground rail on the perfboard, with individual signal wires running to the respective GPIOs through 680Ω resistors.

Mounting Hardware
4× 2mm × 8mm screws: Secure perfboard to enclosure base or standoffs

4× 2mm × 12mm screws: Attach enclosure lid or secure T-Display S3 to mounting plate

Battery Installation
The 3.7V 2000mAh LiPo with JST connector plugs directly into the T-Display S3's battery input. The board includes onboard charging via USB-C.

📄 License
MIT — do whatever you want with it. Build one for your buddy's Christmas present.

🏁 Credits
Built with PlatformIO, LovyanGFX, and a lot of reaction time practice. Inspired by NHRA drag racing Christmas tree timing systems.
