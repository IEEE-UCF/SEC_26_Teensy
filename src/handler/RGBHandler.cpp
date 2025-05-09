#include "RGBHandler.h"

// initializing static member for tracking previous streak positions
int RGBHandler::prev_positions[NUM_SECTIONS];

RGBHandler::RGBHandler(uint8_t kLED)
    : kLED(kLED),
      leds(TOTAL_LEDS, displayMemory, drawingMemory, kLED, WS2812_GRB) {
  // setting all previous positions to -1 (no active streaks)
  for (int i = 0; i < NUM_SECTIONS; i++) {
    prev_positions[i] = -1;
  }
}

bool RGBHandler::Begin() {
  static bool serialInitialized = false;
  if (!serialInitialized) {
    Serial3.begin(115200);
    serialInitialized = true;
  }

  // computing section start indices
  sectionStarts[0] = 0;
  for (int i = 1; i < NUM_SECTIONS; i++) {
    sectionStarts[i] = sectionStarts[i - 1] + SECTION_SIZES[i - 1];
  }

  leds.begin();
  setGlobalBrightness(DEFAULT_BRIGHTNESS);
  return setSectionSolidColor(0, 0, 0,
                              0);  // turning off all sections initially
}

// brightness scaling with rounding correction
void RGBHandler::applyBrightness(uint8_t r, uint8_t g, uint8_t b,
                                 uint8_t &r_out, uint8_t &g_out,
                                 uint8_t &b_out) {
  r_out = constrain(((uint32_t)r * globalBrightness + 127) / 255, 0U, 255U);
  g_out = constrain(((uint32_t)g * globalBrightness + 127) / 255, 0U, 255U);
  b_out = constrain(((uint32_t)b * globalBrightness + 127) / 255, 0U, 255U);
}

bool RGBHandler::setSectionSolidColor(uint8_t section, uint8_t r, uint8_t g,
                                      uint8_t b) {
  if (section >= NUM_SECTIONS) return false;
  stopSectionEffect(section);

  uint8_t r_adj, g_adj, b_adj;
  applyBrightness(r, g, b, r_adj, g_adj, b_adj);

  uint16_t start = sectionStarts[section];
  uint16_t end = start + SECTION_SIZES[section];
  for (uint16_t i = start; i < end; i++) {
    leds.setPixel(i, r_adj, g_adj, b_adj);
  }
  leds.show();
  return true;
}

bool RGBHandler::setSectionSolidColor(uint8_t section, const RGBColor &color) {
  return setSectionSolidColor(section, color.r, color.g, color.b);
}

// setting up pulse effect with smooth transition control
bool RGBHandler::setSectionPulseEffect(uint8_t section, uint8_t r, uint8_t g,
                                       uint8_t b, unsigned long speed) {
  if (section >= NUM_SECTIONS || speed < MIN_SPEED || speed > MAX_SPEED)
    return false;

  SectionEffect &sec = sections[section];
  if (!(sec.currentEffect == NONE)) {
    return false;
  }
  sec.currentEffect = PULSE;
  sec.effectSpeed = speed;
  sec.pulse_r = r;
  sec.pulse_g = g;
  sec.pulse_b = b;

  // initialize phase to start at minimum brightness
  sec.pulse_phase = 0;

  // ensuring the pulse step isn't too fast or too slow
  uint16_t base_step = (1024 * 10UL) / speed;
  sec.pulse_step = max(constrain(base_step, 2, 20), 2);
  // sec.pulse_step = base_step;
  return true;
}

bool RGBHandler::setSectionPulseEffect(uint8_t section, const RGBColor &color,
                                       unsigned long speed) {
  return setSectionPulseEffect(section, color.r, color.g, color.b, speed);
}

// setting up streak effect (forward direction)
bool RGBHandler::setSectionStreakEffect(uint8_t section, uint8_t r, uint8_t g,
                                        uint8_t b, unsigned long speed) {
  // Call the reverse overload with reverse set to false
  return setSectionStreakEffect(section, r, g, b, speed, false);
}

bool RGBHandler::setSectionStreakEffect(uint8_t section, const RGBColor &color,
                                        unsigned long speed) {
  return setSectionStreakEffect(section, color.r, color.g, color.b, speed,
                                false);
}

// overloaded versions that accept a reverse flag
bool RGBHandler::setSectionStreakEffect(uint8_t section, uint8_t r, uint8_t g,
                                        uint8_t b, unsigned long speed,
                                        bool reverse) {
  if (section >= NUM_SECTIONS || speed < MIN_SPEED || speed > MAX_SPEED)
    return false;

  SectionEffect &sec = sections[section];
  if (!(sec.currentEffect == NONE)) {
    return false;
  }
  sec.currentEffect = STREAK;
  sec.effectSpeed = speed;
  sec.streak_r = r;
  sec.streak_g = g;
  sec.streak_b = b;
  sec.reverseStreak = reverse;  // set the reverse flag

  // initialize streak position and trail
  sec.streak_position = 0;
  sec.streak_trailLength = 2;  // default trail length

  // reset previous position tracking
  prev_positions[section] = -1;

  return true;
}

bool RGBHandler::setSectionStreakEffect(uint8_t section, const RGBColor &color,
                                        unsigned long speed, bool reverse) {
  return setSectionStreakEffect(section, color.r, color.g, color.b, speed,
                                reverse);
}

// updating pulse effect
void RGBHandler::updatePulse(uint8_t section) {
  SectionEffect &sec = sections[section];
  // update phase and wrap around at 1024
  sec.pulse_phase += sec.pulse_step;
  sec.pulse_phase %= 1024;

  // calculates brightness using triangle wave
  uint8_t brightness;
  if (sec.pulse_phase < 512) {
    brightness = (sec.pulse_phase * 255) / 511;
  } else {
    brightness = ((1023 - sec.pulse_phase) * 255) / 511;
  }

  // applies pulse brightness to the base color
  uint8_t r = (sec.pulse_r * brightness) / 255;
  uint8_t g = (sec.pulse_g * brightness) / 255;
  uint8_t b = (sec.pulse_b * brightness) / 255;

  // adjust for global brightness
  uint8_t r_adj, g_adj, b_adj;
  applyBrightness(r, g, b, r_adj, g_adj, b_adj);

  // updates all LED's in the section
  uint16_t start = sectionStarts[section];
  uint16_t end = start + SECTION_SIZES[section];
  for (uint16_t i = start; i < end; ++i) {
    leds.setPixel(i, r_adj, g_adj, b_adj);
  }
}

// stops all effects for a specific section
void RGBHandler::stopSectionEffect(uint8_t section) {
  if (section >= NUM_SECTIONS) return;

  SectionEffect &sec = sections[section];
  sec.currentEffect = NONE;

  // turn off all LEDs in the section
  uint16_t start = sectionStarts[section];
  uint16_t end = start + SECTION_SIZES[section];
  for (uint16_t i = start; i < end; ++i) {
    leds.setPixel(i, 0, 0, 0);
  }

  // resets previous positions for streak effect
  prev_positions[section] = -1;
}

// updating streak effect efficiently with reverse support
void RGBHandler::updateStreak(uint8_t section) {
  SectionEffect &sec = sections[section];
  uint16_t size = SECTION_SIZES[section];
  uint16_t start = sectionStarts[section];
  int current_pos = sec.streak_position;

  // clear previous streak if it has moved
  if (prev_positions[section] != -1 && prev_positions[section] != current_pos &&
      prev_positions[section] < size) {
    leds.setPixel(start + prev_positions[section], 0, 0, 0);
  }

  if (!sec.reverseStreak) {
    // forward streak: drawing new streak using current logic
    for (uint8_t i = 0; i < sec.streak_trailLength; i++) {
      int pos = (current_pos - i + size) % size;
      uint16_t led = start + pos;
      leds.setPixel(led, (sec.streak_r * 200) >> 8, (sec.streak_g * 200) >> 8,
                    (sec.streak_b * 200) >> 8);
    }
    prev_positions[section] =
        (current_pos - sec.streak_trailLength + size) % size;
    sec.streak_position = (current_pos + 1) % size;
  } else {
    // reverse streak: drawing new streak in the opposite direction
    for (uint8_t i = 0; i < sec.streak_trailLength; i++) {
      int pos = (current_pos + i) % size;
      uint16_t led = start + pos;
      leds.setPixel(led, (sec.streak_r * 200) >> 8, (sec.streak_g * 200) >> 8,
                    (sec.streak_b * 200) >> 8);
    }
    prev_positions[section] = (current_pos + sec.streak_trailLength) % size;
    // decrement the current position with wrap-around
    sec.streak_position = (current_pos + size - 1) % size;
  }
}

// manages effect updates while avoiding redundant refreshes
void RGBHandler::Update() {
  static unsigned long lastShow = 0;
  bool updated = false;
  unsigned long now = millis();

  for (uint8_t s = 0; s < NUM_SECTIONS; s++) {
    SectionEffect &sec = sections[s];
    if (sec.currentEffect == NONE) continue;

    if (now - sec.lastUpdate >= sec.effectSpeed) {
      if (sec.currentEffect == PULSE) updatePulse(s);
      if (sec.currentEffect == STREAK) updateStreak(s);
      updated = true;
      sec.lastUpdate = now;
    }
  }

  // refreshes the led strip at most 60 times per second
  if (updated && (now - lastShow >= 16)) {
    leds.show();
    lastShow = now;
  }
}

// stops all effects at once
void RGBHandler::stopAllEffects() {
  for (uint8_t s = 0; s < NUM_SECTIONS; s++) stopSectionEffect(s);
  leds.show();
}

bool RGBHandler::setGlobalBrightness(uint8_t brightness) {
  globalBrightness = constrain(brightness, 0, MAX_BRIGHTNESS);
  leds.setBrightness(globalBrightness);
  return true;
}

void RGBHandler::PrintInfo(Print &output, bool printConfig) const {
  if (printConfig) {
    output.print(F("RGBHandler Configuration: "));
    output.print(F("Number of Sections: "));
    output.println(NUM_SECTIONS);
    output.print(F("Section Sizes: "));
    for (int i = 0; i < NUM_SECTIONS; i++) {
      output.print(SECTION_SIZES[i]);
      if (i < NUM_SECTIONS - 1) {
        output.print(F(", "));
      }
    }
    output.println();
    output.print(F("Global Brightness: "));
    output.println(globalBrightness);
  } else {
    output.print(F("Section States: "));
    for (int i = 0; i < NUM_SECTIONS; i++) {
      output.print(F("Section "));
      output.print(i);
      output.print(F(": "));
      output.print(F("Effect: "));
      output.print(sections[i].currentEffect);
      output.print(F(", Last Update: "));
      output.print(sections[i].lastUpdate);
      output.print(F(", Effect Speed: "));
      output.print(sections[i].effectSpeed);
      output.print(F(", Pulse RGB: ("));
      output.print(sections[i].pulse_r);
      output.print(F(", "));
      output.print(sections[i].pulse_g);
      output.print(F(", "));
      output.print(sections[i].pulse_b);
      output.print(F("), Streak RGB: ("));
      output.print(sections[i].streak_r);
      output.print(F(", "));
      output.print(sections[i].streak_g);
      output.print(F(", "));
      output.print(sections[i].streak_b);
      output.print(F("), Streak Position: "));
      output.print(sections[i].streak_position);
      output.print(F(", Streak Trail Length: "));
      output.println(sections[i].streak_trailLength);
    }
  }
}

Print &operator<<(Print &output, const RGBHandler &handler) {
  handler.PrintInfo(output, false);
  return output;
}