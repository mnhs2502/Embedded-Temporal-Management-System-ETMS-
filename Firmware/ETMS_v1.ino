#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// --- PINS ---
const int BTN_MAIN = 2;
const int BTN_MODE = 3;
const int BTN_SKIP = 4;
const int LED_WORK = 10;
const int LED_BREAK = 11;

// --- CONFIG ---
const unsigned long LONG_PRESS_TIME = 10000UL;
const unsigned long DEBOUNCE_TIME = 50UL;
const int MAX_SESSIONS = 7; 

struct TimerMode {
  unsigned long workDuration;
  unsigned long breakDuration;
};

TimerMode modes[2] = {
  {25UL * 60UL, 5UL * 60UL},
  {50UL * 60UL, 10UL * 60UL}
};

bool modeToggle = false;
int currentSession = 1; 

enum Phase { PAUSED, WORK, BREAKTIME };
Phase currentPhase = PAUSED;
Phase previousRunningPhase = WORK;

unsigned long remainingTime = 25UL * 60UL;
unsigned long previousSecondMillis = 0;

struct Button {
  int pin;
  bool lastReading;
  bool stableState;
  unsigned long lastDebounceTime;
  bool longPressTriggered;
  unsigned long pressStartTime;
};

Button btnMain = {BTN_MAIN, HIGH, HIGH, 0, false, 0};
Button btnMode = {BTN_MODE, HIGH, HIGH, 0, false, 0};
Button btnSkip = {BTN_SKIP, HIGH, HIGH, 0, false, 0};

// --- EEPROM ---
struct SaveData {
  byte signature;
  byte phase;
  bool mode;
  unsigned long remain;
  int session;
};
const byte EEPROM_SIGNATURE = 0x5C; // Updated signature for new struct

void setup() {
  pinMode(BTN_MAIN, INPUT_PULLUP);
  pinMode(BTN_MODE, INPUT_PULLUP);
  pinMode(BTN_SKIP, INPUT_PULLUP);
  pinMode(LED_WORK, OUTPUT);
  pinMode(LED_BREAK, OUTPUT);

  lcd.init();
  lcd.backlight();

  loadState();
  updateLEDs();
  updateLCD();
}

void loop() {
  handleButtons();

  if (currentPhase != PAUSED) {
    if (millis() - previousSecondMillis >= 1000) {
      previousSecondMillis = millis();
      if (remainingTime > 0) remainingTime--;
      
      if (remainingTime == 0) {
        if (currentPhase == WORK) switchToBreak();
        else if (currentPhase == BREAKTIME) switchToWork();
      }
      updateLCD();
    }
  }
}

void handleButtons() {
  processButton(btnMain);
  processButton(btnMode);
  processButton(btnSkip);
}

void processButton(Button &btn) {
  bool reading = digitalRead(btn.pin);
  if (reading != btn.lastReading) btn.lastDebounceTime = millis();

  if ((millis() - btn.lastDebounceTime) > DEBOUNCE_TIME) {
    if (reading != btn.stableState) {
      btn.stableState = reading;
      if (btn.stableState == LOW) {
        btn.pressStartTime = millis();
        btn.longPressTriggered = false;
      } else {
        if (!btn.longPressTriggered) {
          if (btn.pin == BTN_MAIN) shortPressMain();
          else if (btn.pin == BTN_SKIP) shortPressSkip();
        }
      }
    }
  }

  if (btn.stableState == LOW && !btn.longPressTriggered) {
    if (millis() - btn.pressStartTime >= LONG_PRESS_TIME) {
      btn.longPressTriggered = true;
      if (btn.pin == BTN_MAIN) resetSystem();
      else if (btn.pin == BTN_MODE) longPressMode();
    }
  }
  btn.lastReading = reading;
}

void shortPressMain() {
  currentPhase = (currentPhase == PAUSED) ? previousRunningPhase : PAUSED;
  if (currentPhase != PAUSED) previousRunningPhase = currentPhase;
  saveState();
  updateLEDs();
  updateLCD();
}

void longPressMode() {
  modeToggle = !modeToggle;
  remainingTime = (currentPhase == BREAKTIME) ? modes[modeToggle].breakDuration : modes[modeToggle].workDuration;
  saveState();
  updateLCD();
}

void shortPressSkip() {
  if (currentPhase == WORK) switchToBreak();
  else switchToWork();
  saveState();
}

void switchToWork() {
  if (currentSession < MAX_SESSIONS) {
    currentSession++;
    currentPhase = WORK;
    previousRunningPhase = WORK;
    remainingTime = modes[modeToggle].workDuration;
  } else {
    resetSystem(); // Completed all 7 sessions
  }
  updateLEDs();
  updateLCD();
}

void switchToBreak() {
  currentPhase = BREAKTIME;
  previousRunningPhase = BREAKTIME;
  remainingTime = modes[modeToggle].breakDuration;
  updateLEDs();
  updateLCD();
}

void updateLCD() {
  lcd.clear();
  
  // Row 0: Status and Session Info
  lcd.setCursor(0, 0);
  if (currentPhase == WORK) {
    lcd.print("SESSION ");
    lcd.setCursor(11, 0);
    lcd.print(currentSession);
    lcd.print("/");
    lcd.print(MAX_SESSIONS);
  } else if (currentPhase == BREAKTIME) {
    lcd.print("     BREAK");
  } else {
    lcd.print("PAUSED");
    lcd.setCursor(11, 0);
    lcd.print(modeToggle ? "50/10" : "25/5");
  }

  // Row 1: Large Centered Timer
  int mins = remainingTime / 60;
  int secs = remainingTime % 60;
  lcd.setCursor(5, 1);
  if (mins < 10) lcd.print("0");
  lcd.print(mins);
  lcd.print(":");
  if (secs < 10) lcd.print("0");
  lcd.print(secs);
}

void updateLEDs() {
  digitalWrite(LED_WORK, currentPhase == WORK);
  digitalWrite(LED_BREAK, currentPhase == BREAKTIME);
}

void saveState() {
  SaveData data = {EEPROM_SIGNATURE, (byte)currentPhase, modeToggle, remainingTime, currentSession};
  EEPROM.put(0, data);
}

void loadState() {
  SaveData data;
  EEPROM.get(0, data);
  if (data.signature == EEPROM_SIGNATURE) {
    currentPhase = (Phase)data.phase;
    previousRunningPhase = (currentPhase == PAUSED) ? WORK : currentPhase;
    modeToggle = data.mode;
    remainingTime = data.remain;
    currentSession = data.session;
  } else {
    resetSystem();
  }
}

void resetSystem() {
  modeToggle = false;
  currentSession = 1;
  currentPhase = PAUSED;
  previousRunningPhase = WORK;
  remainingTime = modes[0].workDuration;
  saveState();
  lcd.clear();
  lcd.print("SYSTEM RESET");
  delay(2000);
  updateLEDs();
  updateLCD();
}
