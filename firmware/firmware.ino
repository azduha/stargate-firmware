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
    Serial.print(symbols[0]); Serial.print(", ");
    Serial.print(symbols[1]); Serial.print(", ");
    Serial.print(symbols[2]); Serial.print(", ");
    Serial.print(symbols[3]); Serial.print(", ");
    Serial.print(symbols[4]); Serial.print(", ");
    Serial.print(symbols[5]); Serial.print(", ");
    Serial.print(symbols[6]); Serial.println("");
    if (symbols[0] == 13 && symbols[1] == 14 && symbols[2] == 15 && symbols[3] == 16 && symbols[4] == 17 && symbols[5] == 18 && symbols[6] == 19) {
      state = 5;
      progress = 0;
    }
  }
  if (state == 5) {
    int modes = 10;
    if (ptr % modes == 0) {
      colorWipe(strip.Color(255,   0,   0), 10);
      colorWipe(strip.Color(  0,   0,   0), 10);
    }
    if (ptr % modes == 1) {
      colorWipe(strip.Color(  0, 255,   0), 10);
      colorWipe(strip.Color(  0,   0,   0), 10);
    }
    if (ptr % modes == 2) {
      colorWipe(strip.Color(  0,   0, 255), 10);
      colorWipe(strip.Color(  0,   0,   0), 10);
    }
    if (ptr % modes == 3) {
      colorWipe(strip.Color(255, 255, 255), 10);
      colorWipe(strip.Color(  0,   0,   0), 10);
    }
    if (ptr % modes == 4) {
      theaterChase(strip.Color(255,   0,   0), 10);
      theaterChase(strip.Color(  0,   0,   0), 10);
    }
    if (ptr % modes == 5) {
      theaterChase(strip.Color(  0, 255,   0), 10);
      theaterChase(strip.Color(  0,   0,   0), 10);
    }
    if (ptr % modes == 6) {
      theaterChase(strip.Color(  0,   0, 255), 10);
      theaterChase(strip.Color(  0,   0,   0), 10);
    }
    if (ptr % modes == 7) {
      theaterChase(strip.Color(255, 255, 255), 10);
      theaterChase(strip.Color(  0,   0,   0), 10);
    }
    if (ptr % modes == 8) {
      strip.rainbow(256 * progress);
    }
    if (ptr % modes == 9) {
      theaterChaseRainbow(50);
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


//// From strandtest.ino - Neopixel

// Some functions of our own for creating animated effects -----------------

// Fill strip pixels one after another with a color. Strip is NOT cleared
// first; anything there will be covered pixel by pixel. Pass in color
// (as a single 'packed' 32-bit value, which you can get by calling
// strip.Color(red, green, blue) as shown in the loop() function above),
// and a delay time (in milliseconds) between pixels.
void colorWipe(uint32_t color, int wait) {
  int ptrLoc = ptr;
  for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
    if (ptr != ptrLoc) return;
  }
}

// Theater-marquee-style chasing lights. Pass in a color (32-bit value,
// a la strip.Color(r,g,b) as mentioned above), and a delay time (in ms)
// between frames.
void theaterChase(uint32_t color, int wait) {
  int ptrLoc = ptr;
  for(int a=0; a<10; a++) {  // Repeat 10 times...
    for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
      strip.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in steps of 3...
      for(int c=b; c<strip.numPixels(); c += 3) {
        strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      strip.show(); // Update strip with new contents
      delay(wait);  // Pause for a moment
      if (ptr != ptrLoc) return;
    }
  }
}

// Rainbow-enhanced theater marquee. Pass delay time (in ms) between frames.
void theaterChaseRainbow(int wait) {
  int ptrLoc = ptr;
  int firstPixelHue = 0;     // First pixel starts at red (hue 0)
  for(int a=0; a<30; a++) {  // Repeat 30 times...
    for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
      strip.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in increments of 3...
      for(int c=b; c<strip.numPixels(); c += 3) {
        // hue of pixel 'c' is offset by an amount to make one full
        // revolution of the color wheel (range 65536) along the length
        // of the strip (strip.numPixels() steps):
        int      hue   = firstPixelHue + c * 65536L / strip.numPixels();
        uint32_t color = strip.gamma32(strip.ColorHSV(hue)); // hue -> RGB
        strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      strip.show();                // Update strip with new contents
      if (ptr != ptrLoc) return;
      delay(wait);                 // Pause for a moment
      firstPixelHue += 65536 / 90; // One cycle of color wheel over 90 frames
    }
  }
}