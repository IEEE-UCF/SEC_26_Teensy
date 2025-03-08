#include "RGBHandler.h"

// initializing static member for tracking previous streak positions
int RGBHandler::prev_positions[NUM_SECTIONS];

RGBHandler::RGBHandler() : 
    leds(TOTAL_LEDS, displayMemory, drawingMemory, LED_PIN, WS2812_GRB) 
{
    // setting all previous positions to -1 (no active streaks)
    for (int i = 0; i < NUM_SECTIONS; i++) {
        prev_positions[i] = -1;
    }
}

bool RGBHandler::begin() {
    static bool serialInitialized = false;
    if (!serialInitialized) {
        Serial.begin(115200);
        serialInitialized = true;
    }

    // computing section start indices
    sectionStarts[0] = 0;
    for (int i = 1; i < NUM_SECTIONS; i++) {
        sectionStarts[i] = sectionStarts[i-1] + SECTION_SIZES[i-1];
    }

    leds.begin();
    setGlobalBrightness(DEFAULT_BRIGHTNESS);
    return setSectionSolidColor(0, 0, 0, 0); // turning off all sections initially
}

// brightness scaling with rounding correction
void RGBHandler::applyBrightness(uint8_t r, uint8_t g, uint8_t b,
                                uint8_t &r_out, uint8_t &g_out, uint8_t &b_out) {
    r_out = constrain(((uint32_t)r * globalBrightness + 127) / 255, 0, 255);
    g_out = constrain(((uint32_t)g * globalBrightness + 127) / 255, 0, 255);
    b_out = constrain(((uint32_t)b * globalBrightness + 127) / 255, 0, 255);
}

bool RGBHandler::setSectionSolidColor(uint8_t section, uint8_t r, uint8_t g, uint8_t b) {
    if (section >= NUM_SECTIONS) return false;
    stopSectionEffect(section);

    uint8_t r_adj, g_adj, b_adj;
    applyBrightness(r, g, b, r_adj, g_adj, b_adj);

    uint16_t start = sectionStarts[section];
    uint16_t end = start + SECTION_SIZES[section];
    for (uint16_t i = start; i < end; i++) {
        leds.setPixel(i, r_adj, g_adj, b_adj);
    }
    return leds.show();
}

// setting up pulse effect with smooth transition control
bool RGBHandler::setSectionPulseEffect(uint8_t section, uint8_t r, uint8_t g, uint8_t b, unsigned long speed) {
    if (section >= NUM_SECTIONS || speed < MIN_SPEED || speed > MAX_SPEED) return false;
    
    SectionEffect &sec = sections[section];
    sec.currentEffect = PULSE;
    sec.effectSpeed = speed;
    sec.pulse_r = r;
    sec.pulse_g = g;
    sec.pulse_b = b;
    
    // ensuring the pulse step isn't too fast or too slow
    uint16_t base_step = (1024 * 10UL) / speed;
    sec.pulse_step = max(constrain(base_step, 2, 20), 2);
    return true;
}

// updating streak effect efficiently
void RGBHandler::updateStreak(uint8_t section) {
    SectionEffect &sec = sections[section];
    uint16_t size = SECTION_SIZES[section];
    uint16_t start = sectionStarts[section];
    int current_pos = sec.streak_position;

    // cleares previous streak if it has moved
    if (prev_positions[section] != -1 && 
        prev_positions[section] != current_pos &&
        prev_positions[section] < size) {
        leds.setPixel(start + prev_positions[section], 0, 0, 0);
    }

    // drawing new streak
    for (uint8_t i = 0; i < sec.streak_trailLength; i++) {
        int pos = (current_pos - i + size) % size;
        if (pos >= size) continue;
        uint16_t led = start + pos;
        leds.setPixel(led,
            (sec.streak_r * 200) >> 8,
            (sec.streak_g * 200) >> 8,
            (sec.streak_b * 200) >> 8
        );
    }

    // tracks position updates
    prev_positions[section] = (current_pos - sec.streak_trailLength + size) % size;
    sec.streak_position = (current_pos + 1) % size;
}

// manages effect updates while avoiding redundant refreshes
void RGBHandler::update() {
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
