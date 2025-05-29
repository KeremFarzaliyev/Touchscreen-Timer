#include <Arduino_GigaDisplay_GFX.h>
#include <Arduino_GigaDisplayTouch.h>

// Display & Touch
GigaDisplay_GFX         display;
Arduino_GigaDisplayTouch touchDetector;

// Ultrasonic sensor pins
const int TRIG_PIN   = 9;
const int ECHO_PIN   = 8;
// Backlight control
const int BL_PIN     = 7;
// Buzzer pin
const int BUZZER_PIN = 30;

// Distance threshold (cm)
const float DIST_THRESHOLD = 10.0;

// Timer state
unsigned long startTime        = 0;
unsigned long pausedTime       = 0;
unsigned long totalPauseStart  = 0;
bool         isRunning         = false;
bool         isPaused          = false;
bool         breakMode         = false;  
unsigned long countdownDuration  = 40UL*60UL*1000UL;
unsigned long countdownRemaining = countdownDuration;

// Button layout
#define BTN_HEIGHT   80
#define BTN_WIDTH    130
#define BTN_Y        380
#define STOP_X       100
#define MIN20_X      300
#define MIN40_X      500
#define BTN_RESUME_X 220
#define BTN_RESUME_Y 200
#define BTN_RESUME_W 160
#define BTN_RESUME_H 70
#define BTN_END_X    420
#define BTN_END_Y    200
#define BTN_END_W    120
#define BTN_END_H     70

// Colors
#define BLACK  0x0000
#define WHITE  0xFFFF
#define RED    0xF800
#define GREEN  0x07E0
#define CYAN   0x07FF
#define YELLOW 0xFFE0

// Prototypes
float  readDistanceCM();
void   updateTimer();
void   drawTimerScreen();
void   drawStopMenu();
bool   touchInRect(int tx,int ty,int rx,int ry,int rw,int rh);

void setup() {
  Serial.begin(115200);

  // Init display
  display.begin();
  display.setRotation(3);

  // Backlight off initially
  pinMode(BL_PIN, OUTPUT);
  digitalWrite(BL_PIN, LOW);

  // Init touch
  if (!touchDetector.begin()) {
    Serial.println("Touch init FAILED");
    while (1);
  }

  // Init ultrasonic
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Init buzzer
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  // Blank screen
  display.fillScreen(BLACK);
}

void loop() {
  // 1) Read distance
  float dist = readDistanceCM();
  Serial.print("Distance: "); Serial.print(dist); Serial.println(" cm");

  // 2) Trigger work phase on removal
  if (!isRunning && !breakMode && dist > DIST_THRESHOLD) {
    digitalWrite(BL_PIN, HIGH);  
    isRunning         = true;
    isPaused          = false;
    countdownDuration = 40UL*60UL*1000UL;
    countdownRemaining= countdownDuration;
    startTime         = millis();
    pausedTime        = 0;
    drawTimerScreen();
  }

  // 3) Trigger break phase on replacement after work done
  if (!isRunning && breakMode && dist < DIST_THRESHOLD) {
    digitalWrite(BL_PIN, HIGH);
    isRunning         = true;
    isPaused          = false;
    countdownDuration = 20UL*60UL*1000UL;
    countdownRemaining= countdownDuration;
    startTime         = millis();
    pausedTime        = 0;
    drawTimerScreen();
  }

  // 4) Update active timer
  if (isRunning && !isPaused) {
    updateTimer();
  }

  // 5) Handle touch buttons
  GDTpoint_t pts[5];
  uint8_t    contacts = touchDetector.getTouchPoints(pts);
  if (contacts) {
    int tx = 800 - pts[0].y - 1;
    int ty = pts[0].x;

    if (!isPaused) {
      // STOP
      if (touchInRect(tx, ty, STOP_X, BTN_Y, BTN_WIDTH, BTN_HEIGHT)) {
        isPaused = true;
        totalPauseStart = millis();
        drawStopMenu();
        delay(300);
      }
      // 20 MIN
      else if (touchInRect(tx, ty, MIN20_X, BTN_Y, BTN_WIDTH, BTN_HEIGHT)) {
        isRunning         = true;
        isPaused          = false;
        countdownDuration = 20UL*60UL*1000UL;
        countdownRemaining= countdownDuration;
        startTime         = millis();
        pausedTime        = 0;
        drawTimerScreen();
        delay(300);
      }
      // 40 MIN
      else if (touchInRect(tx, ty, MIN40_X, BTN_Y, BTN_WIDTH, BTN_HEIGHT)) {
        isRunning         = true;
        isPaused          = false;
        countdownDuration = 40UL*60UL*1000UL;
        countdownRemaining= countdownDuration;
        startTime         = millis();
        pausedTime        = 0;
        drawTimerScreen();
        delay(300);
      }
    } else {
      // RESUME
      if (touchInRect(tx, ty, BTN_RESUME_X, BTN_RESUME_Y, BTN_RESUME_W, BTN_RESUME_H)) {
        pausedTime += millis() - totalPauseStart;
        isPaused = false;
        drawTimerScreen();
        delay(300);
      }
      // END
      else if (touchInRect(tx, ty, BTN_END_X, BTN_END_Y, BTN_END_W, BTN_END_H)) {
        isRunning = false;
        isPaused  = false;
        breakMode = false;
        display.fillScreen(BLACK);
        delay(300);
        digitalWrite(BL_PIN, LOW);  
      }
    }
  }

  delay(100);
}

void updateTimer() {
  unsigned long elapsed = (millis() - startTime) - pausedTime;
  if (elapsed >= countdownDuration) {
    isRunning = false;

    // Buzzer: 2 kHz, 3 s
    tone(BUZZER_PIN, 2000);
    delay(3000);
    noTone(BUZZER_PIN);

    if (!breakMode) {
      // Work complete → await break
      breakMode = true;
      drawTimerScreen();
    } else {
      // Break complete → reset
      breakMode = false;
      drawTimerScreen();
      digitalWrite(BL_PIN, LOW);  
    }
  } else {
    countdownRemaining = countdownDuration - elapsed;
    drawTimerScreen();
  }
}

float readDistanceCM() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long dur = pulseIn(ECHO_PIN, HIGH);
  return (dur * 0.034f) / 2.0f;
}

void drawTimerScreen() {
  display.fillScreen(BLACK);
  display.setTextColor(YELLOW);
  display.setTextSize(3);
  display.setCursor(280,40);
  display.println("TIMER");

  unsigned long secs = countdownRemaining / 1000;
  char buf[16];
  sprintf(buf, "%02lu:%02lu", secs/60, secs%60);

  display.setCursor(220,150);
  display.setTextSize(8);
  display.setTextColor(CYAN);
  display.print(buf);

  // Buttons
  display.drawRect(STOP_X, BTN_Y, BTN_WIDTH, BTN_HEIGHT, RED);
  display.setCursor(STOP_X+25, BTN_Y+25);
  display.setTextSize(2);
  display.setTextColor(RED);
  display.print("STOP");

  display.drawRect(MIN20_X, BTN_Y, BTN_WIDTH, BTN_HEIGHT, GREEN);
  display.setCursor(MIN20_X+10, BTN_Y+25);
  display.setTextSize(2);
  display.setTextColor(GREEN);
  display.print("20 MIN");

  display.drawRect(MIN40_X, BTN_Y, BTN_WIDTH, BTN_HEIGHT, GREEN);
  display.setCursor(MIN40_X+10, BTN_Y+25);
  display.setTextSize(2);
  display.setTextColor(GREEN);
  display.print("40 MIN");
}

void drawStopMenu() {
  display.fillScreen(BLACK);
  display.setTextSize(4);
  display.setTextColor(WHITE);
  display.setCursor(170,100);
  display.println("TIMER STOPPED");

  display.drawRect(BTN_RESUME_X, BTN_RESUME_Y, BTN_RESUME_W, BTN_RESUME_H, GREEN);
  display.setCursor(BTN_RESUME_X+10, BTN_RESUME_Y+25);
  display.setTextSize(3);
  display.setTextColor(GREEN);
  display.print("Resume");

  display.drawRect(BTN_END_X, BTN_END_Y, BTN_END_W, BTN_END_H, RED);
  display.setCursor(BTN_END_X+25, BTN_END_Y+25);
  display.setTextSize(3);
  display.setTextColor(RED);
  display.print("End");
}

bool touchInRect(int tx,int ty,int rx,int ry,int rw,int rh) {
  return tx>=rx && tx<=rx+rw && ty>=ry && ty<=ry+rh;
}
