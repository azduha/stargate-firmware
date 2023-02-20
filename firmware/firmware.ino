#define LED_PIN 6
#define BUTTON_PIN 3
#define ENCODER_1_PIN 4
#define ENCODER_2_PIN 5

#define SYMBOLS 28
#define SELECT_SYMBOLS 7
#define CONTROLLER_OFFSET SYMBOLS * 2

#define LOADING_SPEED 10
#define STARTUP_SPEED 20

#include <Adafruit_NeoPixel.h>

Adafruit_NeoPixel strip(SYMBOLS * 3, LED_PIN, NEO_GRB + NEO_KHZ800);

int ptr = 0;
int progress = 0;
int symbols[SELECT_SYMBOLS] = {};
int selectedSymbols = 0;
int state = -2;
// 0 - Default
// 1 - Selecting
// 2 - Starting
// 3 - Started

int notHeld = 0;

void setup() {
  Serial.begin(9600);

  strip.begin();
  strip.show();
  strip.setBrightness(255);

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(ENCODER_1_PIN, INPUT);
  pinMode(ENCODER_2_PIN, INPUT);
  attachInterrupt(1, doEncoder, CHANGE);
}

void loop() {
  // Apply the states
  if (state == -2) {
    for (int i = 0; i < SYMBOLS; i++) {
      strip.setPixelColor(i * 2, strip.Color(0, 0, 0));
      strip.setPixelColor(i * 2 + 1, strip.Color(0, 0, 0));
      strip.setPixelColor(CONTROLLER_OFFSET + i, strip.Color(0, 0, 0));
    }
    if (digitalRead(BUTTON_PIN) == LOW) {
      // The button press started
      state = -1;
      progress = 0;
    }
  }
  if (state == -1) {
    if (digitalRead(BUTTON_PIN) == HIGH) {
      // The button press started
      state = 0;
      progress = 0;
    }
  }
  if (state == 0) {
    if (digitalRead(BUTTON_PIN) == LOW) {
      // The button press started
      bool check = true;
      for (int j = 0; j < selectedSymbols; j++) {
        if (symbols[j] == ptr) {
          check = false;
        }
      }
      if (check) {
        state = 1;
        progress = 0;
      }
    }

    // Set the LEDs
    for (int i = 0; i < SYMBOLS; i++) {
      if (i == ptr) {
        strip.setPixelColor(CONTROLLER_OFFSET + i, strip.Color(255, 255, 255));
      } else {
        strip.setPixelColor(CONTROLLER_OFFSET + i, strip.Color(0, 0, 0));
        for (int j = 0; j < selectedSymbols; j++) {
          if (i == symbols[j]) {
            strip.setPixelColor(CONTROLLER_OFFSET + i, strip.Color(255, 0, 0));
          }
        }
      }
      strip.setPixelColor(i * 2, strip.Color(0, 0, 0));
      strip.setPixelColor(i * 2 + 1, strip.Color(0, 0, 0));
      for (int j = 0; j < selectedSymbols; j++) {
        strip.setPixelColor(symbols[j] * 2, strip.Color(255, 255, 255));
        strip.setPixelColor(symbols[j] * 2 + 1, strip.Color(255, 255, 255));
      }
    }
  }
  if (state == 1) {
    if (digitalRead(BUTTON_PIN) == HIGH && progress < LOADING_SPEED * SYMBOLS) {
      // The button is released
      state = 0;
      progress = 0;
    }
    if (progress == LOADING_SPEED * (SYMBOLS + 10)) {
      // The button is released
      symbols[selectedSymbols] = ptr;
      selectedSymbols++;

      if (selectedSymbols == SELECT_SYMBOLS) {
        state = 2;
        progress = 0;
      } else {
        state = 0;
        progress = 0;
      }
      Serial.println(ptr);
    }

    // Set the LEDs
    for (int i = 0; i < SYMBOLS; i++) {
      if ((i >= ptr && i <= (ptr + progress / LOADING_SPEED)) || (i <= (ptr + progress / LOADING_SPEED) - SYMBOLS)) {
        strip.setPixelColor(CONTROLLER_OFFSET + i, strip.Color(255, 255, 255));
      } else {
        strip.setPixelColor(CONTROLLER_OFFSET + i, strip.Color(0, 0, 0));
        for (int j = 0; j < selectedSymbols; j++) {
          if (i == symbols[j]) {
            strip.setPixelColor(CONTROLLER_OFFSET + i, strip.Color(255, 0, 0));
          }
        }
      }
      strip.setPixelColor(i * 2, strip.Color(0, 0, 0));
      strip.setPixelColor(i * 2 + 1, strip.Color(0, 0, 0));
      for (int j = 0; j < selectedSymbols; j++) {
        strip.setPixelColor(symbols[j] * 2, strip.Color(255, 255, 255));
        strip.setPixelColor(symbols[j] * 2 + 1, strip.Color(255, 255, 255));
      }
    }
  }
  if (state == 2) {
    if (progress >= 1000) {
      state = 3;
      progress = 0;
    }

    for (int i = 0; i < SYMBOLS; i++) {
      strip.setPixelColor(CONTROLLER_OFFSET + i, strip.Color(0, 0, 0));
      strip.setPixelColor(i * 2, strip.Color(0, 0, 0));
      strip.setPixelColor(i * 2 + 1, strip.Color(0, 0, 0));
    }

    for (int j = 0; j < selectedSymbols; j++) {
      strip.setPixelColor(CONTROLLER_OFFSET + symbols[j], strip.Color(255, 0, 0));
      strip.setPixelColor(symbols[j] * 2, strip.Color(255, 0, 0));
      strip.setPixelColor(symbols[j] * 2 + 1, strip.Color(255, 0, 0));
    }
  }
  if (state == 3) {
    for (int i = 0; i < SYMBOLS; i++) {
      strip.setPixelColor(CONTROLLER_OFFSET + i, strip.Color(0, 0, 0));
      strip.setPixelColor(i * 2, strip.Color(0, 0, 0));
      strip.setPixelColor(i * 2 + 1, strip.Color(0, 0, 0));
    }

    int closed = progress / STARTUP_SPEED;
    for (int i = 0; i < min(closed, SYMBOLS); i++) {
      strip.setPixelColor(i * 2, strip.Color(255, 255, 255));
      strip.setPixelColor(i * 2 + 1, strip.Color(255, 255, 255));

      strip.setPixelColor(CONTROLLER_OFFSET - i * 2 - 1, strip.Color(255, 255, 255));
      strip.setPixelColor(CONTROLLER_OFFSET - i * 2 - 2, strip.Color(255, 255, 255));
    }

    for (int j = 0; j < selectedSymbols; j++) {
      strip.setPixelColor(CONTROLLER_OFFSET + symbols[j], strip.Color(255, 0, 0));
      strip.setPixelColor(symbols[j] * 2, strip.Color(255, 0, 0));
      strip.setPixelColor(symbols[j] * 2 + 1, strip.Color(255, 0, 0));
    }

    if (closed > SYMBOLS) {
      state = 4;
      progress = 0;
    }
  }
  if (state == 4) {
    // Party sequence
    if (symbols[0] == 13 && symbols[1] == 14 && symbols[2] == 15 && symbols[3] == 16 && symbols[4] == 17 && symbols[5] == 18 && symbols[6] == 19) {
      state = 5;
      progress = 0;
    }
  }
  if (state == 5) {
    for (long firstPixelHue = 0; firstPixelHue < 5 * 65536; firstPixelHue += 256) {
      strip.rainbow(256 * progress);
      strip.show();
    }
  }


  // Tick the progress
  progress++;
  if (digitalRead(BUTTON_PIN) == LOW) {
    notHeld = 0;
  } else {
    notHeld++;
  }

  // Do this as the last thing
  strip.show();
}

void doEncoder() {
  if (digitalRead(BUTTON_PIN) == LOW || notHeld < 5) {
    return;
  }
  if (digitalRead(ENCODER_1_PIN) == digitalRead(ENCODER_2_PIN)) {
    ptr++;
    if (ptr >= SYMBOLS) {
      ptr -= SYMBOLS;
    }
  } else {
    ptr--;
    if (ptr < 0) {
      ptr += SYMBOLS;
    }
  }  
}