// Core2 LED bar test: M5GO Bottom2 = 10x SK6812 on GPIO25 (GRB), 5V from M-Bus.
// Phase A drives GPIO25, phase B GPIO15 (M5GO/Fire base, in case the base differs).
#include <M5Unified.h>
#include <FastLED.h>
#define N 10
static CRGB a[N], b[N];

static void showBoth() { FastLED.show(); }
static void fill(CRGB* s, CRGB c) { for (int i = 0; i < N; i++) s[i] = c; }

static void sweep(CRGB* s, int pin) {
  Serial.printf("PIN %d: each LED R,G,B\n", pin);
  const CRGB cols[3] = {CRGB::Red, CRGB::Green, CRGB::Blue};
  for (int c = 0; c < 3; c++)
    for (int i = 0; i < N; i++) { fill(s, CRGB::Black); s[i] = cols[c]; showBoth(); delay(120); }
  Serial.printf("PIN %d: mixes\n", pin);
  const CRGB mix[] = {CRGB(255,80,0), CRGB(0,220,60), CRGB(40,80,255), CRGB::Yellow, CRGB::Cyan, CRGB::Magenta, CRGB::White};
  for (CRGB m : mix) { fill(s, m); showBoth(); delay(500); }
  fill(s, CRGB::Black); showBoth();
}

void setup() {
  auto cfg = M5.config();
  cfg.output_power = true;           // AXP192 EXTEN -> 5V on M-Bus
  M5.begin(cfg);
  M5.Power.setExtOutput(true);       // explicit
  Serial.begin(115200);
  FastLED.addLeds<SK6812, 25, GRB>(a, N);
  FastLED.addLeds<SK6812, 15, GRB>(b, N);
  FastLED.setBrightness(40);
  M5.Display.fillScreen(TFT_BLACK);
  M5.Display.setTextSize(2);
  Serial.printf("LEDTEST ext5v=%d bat=%d%%\n", (int)M5.Power.getExtOutput(), (int)M5.Power.getBatteryLevel());
}

void loop() {
  M5.Display.setCursor(0, 20); M5.Display.fillRect(0, 20, 320, 40, TFT_BLACK);
  M5.Display.print("LED test: GPIO25"); sweep(a, 25);
  M5.Display.setCursor(0, 20); M5.Display.fillRect(0, 20, 320, 40, TFT_BLACK);
  M5.Display.print("LED test: GPIO15"); sweep(b, 15);
}
