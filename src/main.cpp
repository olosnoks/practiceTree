#include <Arduino.h>
#define LGFX_USE_V1
#include "LGFX_TDisplayS3.h"


// ── Display ───────────────────────────────────────────────────────
static LGFX_TDisplayS3 lcd;
static LGFX_Sprite      spr(&lcd);
constexpr int DISPLAY_POWER_PIN = 15;
int  redLightCount = 0;
bool proTreeMode   = false;


// Tap detection for mode switching
int           tapCount    = 0;
unsigned long lastTapTime = 0;
const unsigned long TAP_WINDOW_MS    = 1500;
const unsigned long PRE_STAGE_MS     = 500;


unsigned long tachStartTime = 0;
bool tachAnimating = false;


// =================================================================
// GAME CODE
// =================================================================


const int PIN_STAGE = 1;
const int PIN_Y1    = 2;
const int PIN_Y2    = 3;
const int PIN_Y3    = 10;
const int PIN_GREEN = 11;
const int PIN_RED   = 12;
const int PIN_BTN   = 13;


enum TreeState {
  IDLE,
  PRE_STAGE,
  STAGING,
  Y1_ON,
  Y2_ON,
  Y3_ON,
  PRO_YELLOW,
  GREEN_ON_HELD,
  RED_END,
  GREEN_END
};


TreeState state = IDLE;


unsigned long stateStartTime = 0;
unsigned long STAGE_DELAY_MS = 2000;
unsigned long STEP_DELAY_MS  = 500;


unsigned long greenOnTimeMs  = 0;
unsigned long lastReactionMs = 0;
unsigned long bestReactionMs = 0;   // best (lowest) reaction time ever recorded


const int HISTORY_SIZE = 12;
unsigned long reactionHistory[HISTORY_SIZE] = {0};
int historyCount = 0;


bool buttonPressed() {
  return digitalRead(PIN_BTN) == LOW;
}


void allOff() {
  digitalWrite(PIN_STAGE, LOW);
  digitalWrite(PIN_Y1, LOW);
  digitalWrite(PIN_Y2, LOW);
  digitalWrite(PIN_Y3, LOW);
  digitalWrite(PIN_GREEN, LOW);
  digitalWrite(PIN_RED, LOW);
}


void addReactionToHistory(unsigned long rt) {
  for (int i = HISTORY_SIZE - 1; i > 0; --i) {
    reactionHistory[i] = reactionHistory[i - 1];
  }
  reactionHistory[0] = rt;
  if (historyCount < HISTORY_SIZE) historyCount++;

  if (bestReactionMs == 0 || rt < bestReactionMs) {
    bestReactionMs = rt;
  }
}


float averageReaction() {
  if (historyCount == 0) return 0.0f;
  unsigned long sum = 0;
  for (int i = 0; i < historyCount; ++i) sum += reactionHistory[i];
  return (float)sum / (float)historyCount;
}



// Format reaction time as ".XXX" (sub-second) or "1.XXX" (1s+)
void formatRT(char* buf, int bufSize, unsigned long ms) {
  snprintf(buf, bufSize, "%.3f", ms / 1000.0f);
  if (buf[0] == '0') memmove(buf, buf + 1, strlen(buf)); // strip leading zero
}

// =================================================================
// DISPLAY LAYER
// =================================================================


void drawHeader() {
  const int W = spr.width();
  spr.fillRect(0, 0, W, 36, 0x1A1A1A);
  spr.setTextDatum(MC_DATUM);
  spr.setFont(&fonts::FreeSansBold9pt7b);
  spr.setTextSize(1);
  spr.setTextColor(0x4CAF50);
  spr.drawString(proTreeMode ? "PRO TREE" : "FULL TREE", W / 2, 18);
}


void drawDisplay() {
  const int W = spr.width();
  const int H = spr.height();

  spr.fillSprite(0x0A0A0A);
  char buf[16];


  if (state == GREEN_END) {

    drawHeader();
    spr.setTextDatum(MC_DATUM);
    spr.setFont(&fonts::FreeSans9pt7b);
    spr.setTextColor(0x666666);
    spr.drawString("REACTION TIME", W / 2, 80);

    spr.setFont(&fonts::FreeSansBold9pt7b);
    spr.setTextSize(2);
    spr.setTextColor(TFT_WHITE);
    formatRT(buf, sizeof(buf), lastReactionMs);
    spr.drawString(buf, W / 2, 120);


    spr.setTextColor(0x333333);
    spr.drawString("press for history", W / 2, H - 20);


  } else if (state == IDLE || state == PRE_STAGE) {

    drawHeader();

    // ── Stats row: AVG (left) + BEST (right) ──────────────────────
    const int colL = W / 4;
    const int colR = (W * 3) / 4;

    const int statsLabelY = 54;
    const int statsValueY = 82;

    spr.setFont(&fonts::FreeSans9pt7b);
    spr.setTextSize(1);
    spr.setTextDatum(MC_DATUM);

    spr.setTextColor(0x555555);
    spr.drawString("AVERAGE", colL, statsLabelY);
    spr.setTextColor(TFT_WHITE);
    spr.drawString("BEST", colR, statsLabelY);

    spr.setFont(&fonts::FreeSansBold9pt7b);
    spr.setTextSize(2);

    if (historyCount > 0) {
      spr.setTextColor(TFT_WHITE);
      formatRT(buf, sizeof(buf), (unsigned long)averageReaction());
    } else {
      spr.setTextColor(0x333333);
      snprintf(buf, sizeof(buf), "---");
    }
    spr.drawString(buf, colL, statsValueY);

    if (bestReactionMs > 0) {
      spr.setTextColor(TFT_WHITE);
      formatRT(buf, sizeof(buf), bestReactionMs);
    } else {
      spr.setTextColor(0x333333);
      snprintf(buf, sizeof(buf), "---");
    }
    spr.drawString(buf, colR, statsValueY);

    spr.setTextSize(1);
    spr.setFont(&fonts::FreeSans9pt7b);

    spr.drawFastVLine(W / 2, 48, 68, 0x222222);

    // ── Footer pinned to bottom ────────────────────────────────────
    const int footerLineY = H - 34;
    const int footerTextY = H - 24;

    spr.drawFastHLine(16, footerLineY, W - 32, 0x2A2A2A);
    spr.setTextDatum(TL_DATUM);
    spr.setTextColor(0x555555);
    spr.setFont(&fonts::FreeSans9pt7b);
    spr.drawString("RED LIGHTS", 16, footerTextY);
    spr.setTextDatum(TR_DATUM);
    spr.setTextColor(TFT_WHITE);
    snprintf(buf, sizeof(buf), "%d", redLightCount);
    spr.drawString(buf, W - 16, footerTextY);

    // ── Recent runs section (2x5 grid) ────────────────────────────
    const int recentTitleY = 128;
    const int recentLineY  = 140;
    const int rowY0        = 154;
    const int rowH         = 18;

    spr.drawFastHLine(16, recentTitleY - 8, W - 32, 0x2A2A2A);
    spr.setTextColor(0x555555);
    spr.setTextDatum(TL_DATUM);
    spr.drawString("RECENT RUNS (ms)", 16, recentTitleY);
    spr.drawFastHLine(16, recentLineY, W - 32, 0x222222);

    int count = min(historyCount, HISTORY_SIZE);
    for (int i = 0; i < count; i++) {
      int  row     = i / 2;
      bool leftCol = (i % 2 == 0);
      int x = leftCol ? colL : colR;
      int y = rowY0 + row * rowH;

      formatRT(buf, sizeof(buf), reactionHistory[i]);
      spr.setTextColor(reactionHistory[i] == bestReactionMs ? (uint32_t)0x6699CC : (uint32_t)TFT_WHITE);
      spr.setTextDatum(MC_DATUM);
      spr.drawString(buf, x, y);
    }

    int historyHeight = footerLineY - rowY0 - 4;
    if (historyHeight > 0) {
      spr.drawFastVLine(W / 2, recentLineY, historyHeight, 0x222222);
    }


  } else if (state == RED_END) {

    spr.setTextDatum(MC_DATUM);
    spr.setTextColor(TFT_RED);
    spr.setTextSize(2);
    spr.setFont(&fonts::FreeSansBold9pt7b);
    spr.drawString("RED", W / 2, H / 2 - 24);
    spr.drawString("LIGHT", W / 2, H / 2 + 4);
    spr.setTextSize(1);
    spr.setTextColor(0x555555);
    spr.setFont(&fonts::FreeSans9pt7b);
    spr.drawString("Press to reset", W / 2, H / 2 + 46);


  } else {

    // STAGING, Y1_ON, Y2_ON, Y3_ON, PRO_YELLOW, GREEN_ON_HELD
    drawHeader();
    spr.setTextDatum(MC_DATUM);
    spr.setTextColor(0xFFCC00);
    spr.setTextSize(2);
    spr.setFont(&fonts::FreeSansBold9pt7b);
    spr.drawString("STAGING", W / 2, H / 2 - 14);
    spr.setTextSize(1);
    spr.setTextColor(0x444444);
    spr.setFont(&fonts::FreeSans9pt7b);
    spr.drawString("Hold the button...", W / 2, H / 2 + 22);

  }

  spr.pushSprite(0, 0);
}


// =================================================================
// SETUP & LOOP
// =================================================================


void setup() {
  pinMode(DISPLAY_POWER_PIN, OUTPUT);
  digitalWrite(DISPLAY_POWER_PIN, HIGH);
  delay(10);

  lcd.init();
  lcd.setRotation(2);
  lcd.setBrightness(200);
  spr.setColorDepth(16);
  spr.createSprite(lcd.width(), lcd.height());

  Serial.begin(115200);
  delay(500);

  pinMode(PIN_STAGE, OUTPUT);
  pinMode(PIN_Y1,    OUTPUT);
  pinMode(PIN_Y2,    OUTPUT);
  pinMode(PIN_Y3,    OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_RED,   OUTPUT);
  pinMode(PIN_BTN,   INPUT_PULLUP);

  allOff();
  drawDisplay();

  Serial.println("=== Drag tree with reaction time ===");
  Serial.println("Release BEFORE green -> red only.");
  Serial.println("Release AFTER green -> green only + reaction time.");
}


void loop() {
  unsigned long now = millis();
  bool pressed = buttonPressed();

  TreeState prevState = state;

  switch (state) {

    case IDLE:
      allOff();
      if (pressed) {
        Serial.println("[IDLE] -> PRE_STAGE");
        stateStartTime = now;
        state = PRE_STAGE;
      }
      break;

    case PRE_STAGE:
      if (!pressed) {
        Serial.println("[PRE_STAGE] Released quickly -> IDLE (tap)");
        state = IDLE;
        break;
      }
      if (now - stateStartTime >= PRE_STAGE_MS) {
        Serial.println("[PRE_STAGE] 500ms held -> STAGING");
        STAGE_DELAY_MS = random(1000, 7001);
        Serial.print("[PRE_STAGE] Stage delay = ");
        Serial.print(STAGE_DELAY_MS);
        Serial.println(" ms");
        digitalWrite(PIN_STAGE, HIGH);
        stateStartTime = now;
        state = STAGING;
      }
      break;

    case STAGING:
      digitalWrite(PIN_STAGE, HIGH);
      if (!pressed) {
        Serial.println("[STAGING] Released early -> IDLE");
        state = IDLE;
        break;
      }
      if (now - stateStartTime >= STAGE_DELAY_MS) {
        Serial.println("[STAGING] delay elapsed -> Y1_ON");
        digitalWrite(PIN_Y1, HIGH);
        stateStartTime = now;
        state = Y1_ON;
      }
      break;

    case PRO_YELLOW:
      digitalWrite(PIN_STAGE, HIGH);
      digitalWrite(PIN_Y1, HIGH);
      digitalWrite(PIN_Y2, HIGH);
      digitalWrite(PIN_Y3, HIGH);
      if (!pressed) {
        Serial.println("[PRO_YELLOW] Released early -> RED_END");
        allOff();
        digitalWrite(PIN_RED, HIGH);
        state = RED_END;
        break;
      }
      if (now - stateStartTime >= STEP_DELAY_MS) {
        Serial.println("[PRO_YELLOW] 400ms elapsed -> GREEN_ON_HELD");
        digitalWrite(PIN_GREEN, HIGH);
        greenOnTimeMs = now;
        state = GREEN_ON_HELD;
      }
      break;

    case Y1_ON:
      digitalWrite(PIN_STAGE, HIGH);
      digitalWrite(PIN_Y1, HIGH);
      if (!pressed) {
        Serial.println("[Y1_ON] Released -> RED_END");
        allOff();
        digitalWrite(PIN_RED, HIGH);
        state = RED_END;
        break;
      }
      if (now - stateStartTime >= STEP_DELAY_MS) {
        Serial.println("[Y1_ON] elapsed -> Y2_ON");
        digitalWrite(PIN_Y2, HIGH);
        stateStartTime = now;
        state = Y2_ON;
      }
      break;

    case Y2_ON:
      digitalWrite(PIN_STAGE, HIGH);
      digitalWrite(PIN_Y1, HIGH);
      digitalWrite(PIN_Y2, HIGH);
      if (!pressed) {
        Serial.println("[Y2_ON] Released -> RED_END");
        allOff();
        digitalWrite(PIN_RED, HIGH);
        state = RED_END;
        break;
      }
      if (now - stateStartTime >= STEP_DELAY_MS) {
        Serial.println("[Y2_ON] elapsed -> Y3_ON");
        digitalWrite(PIN_Y3, HIGH);
        stateStartTime = now;
        state = Y3_ON;
      }
      break;

    case Y3_ON:
      digitalWrite(PIN_STAGE, HIGH);
      digitalWrite(PIN_Y1, HIGH);
      digitalWrite(PIN_Y2, HIGH);
      digitalWrite(PIN_Y3, HIGH);
      if (!pressed) {
        Serial.println("[Y3_ON] Released -> RED_END");
        allOff();
        digitalWrite(PIN_RED, HIGH);
        state = RED_END;
        break;
      }
      if (now - stateStartTime >= STEP_DELAY_MS) {
        Serial.println("[Y3_ON] elapsed -> GREEN_ON_HELD");
        digitalWrite(PIN_GREEN, HIGH);
        greenOnTimeMs = now;
        state = GREEN_ON_HELD;
      }
      break;

    case GREEN_ON_HELD:
      digitalWrite(PIN_STAGE, HIGH);
      digitalWrite(PIN_Y1,    HIGH);
      digitalWrite(PIN_Y2,    HIGH);
      digitalWrite(PIN_Y3,    HIGH);
      digitalWrite(PIN_GREEN, HIGH);
      if (!pressed) {
        unsigned long releaseTime = now;
        lastReactionMs = releaseTime - greenOnTimeMs;
        addReactionToHistory(lastReactionMs);
        Serial.print("[GREEN_ON_HELD] Good hit, reaction = ");
        Serial.print(lastReactionMs);
        Serial.print(" ms, avg = ");
        Serial.print(averageReaction());
        Serial.print(" ms, best = ");
        Serial.print(bestReactionMs);
        Serial.println(" ms");
        allOff();
        digitalWrite(PIN_GREEN, HIGH);
        state = GREEN_END;
      }
      break;

    case RED_END:
      if (pressed) {
        Serial.println("[RED_END] Reset -> IDLE");
        state = IDLE;
      }
      break;

    case GREEN_END:
      if (pressed) {
        Serial.println("[GREEN_END] Reset -> IDLE");
        state = IDLE;
      }
      break;
  }


  // ── State change hook (display layer only) ────────────────────────
  if (state != prevState) {

    if (prevState == PRE_STAGE && state == IDLE) {
      if (now - lastTapTime < TAP_WINDOW_MS) {
        tapCount++;
      } else {
        tapCount = 1;
      }
      lastTapTime = now;

      if (tapCount >= 3) {
        proTreeMode   = !proTreeMode;
        STEP_DELAY_MS = proTreeMode ? 100 : 500;
        tapCount      = 0;
        Serial.print("[MODE] Switched to ");
        Serial.println(proTreeMode ? "PRO TREE (400ms simultaneous)" : "FULL TREE (500ms sequence)");
      }
    }

    if (state == GREEN_END || state == RED_END) tapCount = 0;
    if (state == RED_END) redLightCount++;

    // Skip no-op redraw on IDLE -> PRE_STAGE (display is identical to IDLE)
    if (!(prevState == IDLE && state == PRE_STAGE)) {
      drawDisplay();
    }
  }

  delay(1);
}