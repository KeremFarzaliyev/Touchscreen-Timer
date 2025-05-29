#include <Arduino_GigaDisplay_GFX.h>
#include <Arduino_GigaDisplayTouch.h>

// Display & touch
GigaDisplay_GFX         display;
Arduino_GigaDisplayTouch touch;

// Pins
const int BUTTON_PIN = 2;   // tactile switch → D2
const int BUZZER_PIN = 30;  // buzzer → D30

// Layout coords 
#define BTN_Y     380
#define BTN_H     80
#define BTN_W     130
#define STOP_X    100
#define MIN20_X   300
#define MIN40_X   500
#define RES_X     220
#define RES_Y     200
#define RES_W     160
#define RES_H     70
#define END_X     420
#define END_Y     200
#define END_W     120
#define END_H     70

// Colors
#define BLACK   0x0000
#define WHITE   0xFFFF
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define YELLOW  0xFFE0

// App phases
enum Phase { IDLE, RUN_WORK, PAUSE_WORK, WAIT_BREAK, RUN_BREAK, PAUSE_BREAK };
Phase phase = IDLE;

// Timing
unsigned long startTime    = 0;
unsigned long pauseStart   = 0;
unsigned long pausedTotal  = 0;
unsigned long targetMs     = 0;
unsigned long remainMs     = 0;

// Durations
const unsigned long WORK_MS  = 40UL * 60UL * 1000UL;
const unsigned long BREAK_MS = 20UL * 60UL * 1000UL;

// Tactile switch edge‐detect
bool lastSwitch = HIGH;

// Helpers
bool touchInRect(int tx,int ty,int x,int y,int w,int h) {
  return tx>=x && tx<=x+w && ty>=y && ty<=y+h;
}
void beep() {
  tone(BUZZER_PIN, 2000);
  delay(3000);
  noTone(BUZZER_PIN);
}

// Draw countdown screen
void drawTimer() {
  display.fillScreen(BLACK);
  display.setTextColor(YELLOW); display.setTextSize(3);
  display.setCursor(280,40); display.println("TIMER");

  unsigned long s = remainMs / 1000;
  char buf[16];
  sprintf(buf, "%02lu:%02lu", s/60, s%60);

  display.setTextColor(CYAN); display.setTextSize(8);
  display.setCursor(220,150); display.print(buf);

  // STOP
  display.drawRect(STOP_X, BTN_Y, BTN_W, BTN_H, RED);
  display.setTextColor(RED); display.setTextSize(2);
  display.setCursor(STOP_X+25, BTN_Y+25); display.print("STOP");

  // 20 MIN
  display.drawRect(MIN20_X, BTN_Y, BTN_W, BTN_H, GREEN);
  display.setTextColor(GREEN);
  display.setCursor(MIN20_X+10, BTN_Y+25); display.print("20 MIN");

  // 40 MIN
  display.drawRect(MIN40_X, BTN_Y, BTN_W, BTN_H, GREEN);
  display.setTextColor(GREEN);
  display.setCursor(MIN40_X+10, BTN_Y+25); display.print("40 MIN");
}

// Draw STOP/Resume/End menu
void drawPauseMenu() {
  display.fillScreen(BLACK);
  display.setTextSize(4); display.setTextColor(WHITE);
  display.setCursor(170,100); display.println("TIMER STOPPED");

  // Resume
  display.drawRect(RES_X, RES_Y, RES_W, RES_H, GREEN);
  display.setTextSize(3); display.setTextColor(GREEN);
  display.setCursor(RES_X+10, RES_Y+25); display.print("Resume");

  // End
  display.drawRect(END_X, END_Y, END_W, END_H, RED);
  display.setTextColor(RED);
  display.setCursor(END_X+25, END_Y+25); display.print("End");
}

// Kick off a phase
void startPhase(Phase p, unsigned long ms) {
  phase = p;
  targetMs = ms;
  startTime = millis();
  pausedTotal = 0;
  remainMs = ms;
  drawTimer();
}

void setup() {
  Serial.begin(115200);

  // Init display & touch
  display.begin();
  display.setRotation(1);     
  touch.begin();

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  // Idle: draw black once
  display.fillScreen(BLACK);
}

void loop() {
  // 1) Pencil‐click switch starts phases
  bool sw = digitalRead(BUTTON_PIN);
  if (lastSwitch == HIGH && sw == LOW) {
    if (phase == IDLE) {
      startPhase(RUN_WORK, WORK_MS);
    } else if (phase == WAIT_BREAK) {
      startPhase(RUN_BREAK, BREAK_MS);
    }
  }
  lastSwitch = sw;

  // 2) Touch handling, mapped for rotation=1
  GDTpoint_t pts[5];
  if (touch.getTouchPoints(pts)) {
    int rawX = pts[0].x;
    int rawY = pts[0].y;

    // rotation=1 mapping:
    int tx = rawY;
    int ty = display.height() - rawX - 1;

    if (phase == RUN_WORK || phase == RUN_BREAK) {
      if (touchInRect(tx,ty,STOP_X,BTN_Y,BTN_W,BTN_H)) {
        pauseStart = millis();
        phase = (phase==RUN_WORK)? PAUSE_WORK : PAUSE_BREAK;
        drawPauseMenu();
      }
      else if (touchInRect(tx,ty,MIN20_X,BTN_Y,BTN_W,BTN_H)) {
        startPhase(RUN_BREAK, BREAK_MS);
      }
      else if (touchInRect(tx,ty,MIN40_X,BTN_Y,BTN_W,BTN_H)) {
        startPhase(RUN_WORK, WORK_MS);
      }
    }
    else if (phase == PAUSE_WORK || phase == PAUSE_BREAK) {
      if (touchInRect(tx,ty,RES_X,RES_Y,RES_W,RES_H)) {
        pausedTotal += millis() - pauseStart;
        phase = (phase==PAUSE_WORK)? RUN_WORK : RUN_BREAK;
        drawTimer();
      }
      else if (touchInRect(tx,ty,END_X,END_Y,END_W,END_H)) {
        phase = IDLE;
        display.fillScreen(BLACK);
      }
    }
  }

  // 3) Countdown update
  if (phase == RUN_WORK || phase == RUN_BREAK) {
    unsigned long elapsed = millis() - startTime - pausedTotal;
    if (elapsed >= targetMs) {
      remainMs = 0; drawTimer(); beep();
      if (phase == RUN_WORK) {
        phase = WAIT_BREAK;
        drawTimer();  // show 00:00
      } else {
        phase = IDLE;
        display.fillScreen(BLACK);
      }
    } else {
      remainMs = targetMs - elapsed;
      drawTimer();
    }
  }

  delay(50);
}
