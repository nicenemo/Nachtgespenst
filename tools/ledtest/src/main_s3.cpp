// CoreS3 LED bar pin finder: drives the first pixel of a WS2812/SK6812 strip on candidate pins.
// Candidates = every M-Bus pin M5Unified knows for this board plus the Grove ports A/B/C.
// Serial protocol (115200, USB CDC): "LIST" prints candidates, "PIN <gpio>" shows R,G,B on that pin
// (0.6 s each) and answers "OK PIN <gpio>". A host script films the board and finds the pin that lights.
#include <M5Unified.h>

struct Cand { int gpio; const char* label; };
static Cand cands[48];
static int nCands = 0;

static void addCand(int gpio, const char* label) {
  if (gpio < 0 || gpio > 48) return;
  for (int i = 0; i < nCands; i++) if (cands[i].gpio == gpio) return; // dedupe, first label wins
  if (nCands < 48) cands[nCands++] = {gpio, label};
}

static void buildCands() {
  using pn = m5::pin_name_t;
  static const char* const mbusNames[30] = {
    "mbus1","mbus2","mbus3","mbus4","mbus5","mbus6","mbus7","mbus8","mbus9","mbus10",
    "mbus11","mbus12","mbus13","mbus14","mbus15","mbus16","mbus17","mbus18","mbus19","mbus20",
    "mbus21","mbus22","mbus23","mbus24","mbus25","mbus26","mbus27","mbus28","mbus29","mbus30"};
  for (int i = 0; i < 30; i++) addCand(M5.getPin((pn)((int)pn::mbus_pin1 + i)), mbusNames[i]);
  addCand(M5.getPin(pn::port_a_pin1), "portA1"); addCand(M5.getPin(pn::port_a_pin2), "portA2");
  addCand(M5.getPin(pn::port_b_pin1), "portB1"); addCand(M5.getPin(pn::port_b_pin2), "portB2");
  addCand(M5.getPin(pn::port_c_pin1), "portC1"); addCand(M5.getPin(pn::port_c_pin2), "portC2");
}

static void showPin(int gpio) {
  const uint8_t cols[3][3] = {{80, 0, 0}, {0, 80, 0}, {0, 0, 80}};
  for (auto& c : cols) { rgbLedWrite(gpio, c[0], c[1], c[2]); delay(600); }
  rgbLedWrite(gpio, 0, 0, 0);
  pinMode(gpio, INPUT); // release the pin again
}

void setup() {
  auto cfg = M5.config();
  M5.begin(cfg);
  M5.Power.setExtOutput(true); // 5V on the M-Bus
  Serial.begin(115200);
  buildCands();
  M5.Display.fillScreen(TFT_BLACK);
  M5.Display.setTextSize(2);
  M5.Display.setCursor(0, 20);
  M5.Display.print("LED pin finder");
  Serial.printf("LEDTEST-S3 ready cands=%d ext5v=%d\n", nCands, (int)M5.Power.getExtOutput());
}

void loop() {
  static char buf[32];
  static size_t len = 0;
  while (Serial.available()) {
    char ch = (char)Serial.read();
    if (ch == '\n' || ch == '\r') {
      buf[len] = 0;
      if (!strcmp(buf, "LIST")) {
        for (int i = 0; i < nCands; i++) Serial.printf("CAND %d %s\n", cands[i].gpio, cands[i].label);
        Serial.println("OK LIST");
      } else if (!strncmp(buf, "PIN ", 4)) {
        int g = atoi(buf + 4);
        bool known = false;
        for (int i = 0; i < nCands; i++) if (cands[i].gpio == g) known = true;
        if (!known) Serial.println("ERR not a candidate");
        else {
          M5.Display.fillRect(0, 60, 320, 40, TFT_BLACK);
          M5.Display.setCursor(0, 60); M5.Display.printf("GPIO %d", g);
          showPin(g);
          Serial.printf("OK PIN %d\n", g);
        }
      } else if (len) Serial.println("ERR unknown");
      len = 0;
    } else if (len < sizeof(buf) - 1) buf[len++] = ch;
  }
  delay(2);
}
