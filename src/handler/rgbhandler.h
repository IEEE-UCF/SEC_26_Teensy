#ifndef RGBHANDLER_H
#define RGBHANDLER_H

#include <Arduino.h>
#include <WS2812Serial.h>

// defines 7 sections with respective led counts
constexpr uint8_t NUM_SECTIONS = 7;
constexpr uint16_t SECTION_SIZES[NUM_SECTIONS] = {13, 10, 10, 10, 13, 4, 4}; // example sizes, change later idfk

// compile-time calculation of total led count
constexpr uint16_t TOTAL_LEDS = []()
{
    uint16_t total = 0;
    for (auto size : SECTION_SIZES)
        total += size;
    return total;
}();

// pin and brightness settings
constexpr uint8_t LED_PIN = 6;
constexpr unsigned long DEFAULT_SPEED = 50; // default effect speed
constexpr unsigned long MIN_SPEED = 20;
constexpr unsigned long MAX_SPEED = 2000;
constexpr uint8_t DEFAULT_BRIGHTNESS = 128;
constexpr uint8_t MAX_BRIGHTNESS = 255;

// effect types
enum EffectType
{
    NONE,
    PULSE,
    STREAK
};

class RGBHandler
{
public:
    RGBHandler();
    bool begin();
    void update();

    // section-based led control
    bool setSectionSolidColor(uint8_t section, uint8_t r, uint8_t g, uint8_t b);
    bool setSectionPulseEffect(uint8_t section, uint8_t r, uint8_t g, uint8_t b, unsigned long speed);
    bool setSectionStreakEffect(uint8_t section, uint8_t r, uint8_t g, uint8_t b, unsigned long speed);
    bool stopSectionEffect(uint8_t section);
    bool setGlobalBrightness(uint8_t brightness);
    bool processCommand(const String &command);
    void stopAllEffects();

private:
    // storing effect states for each section
    struct SectionEffect {
    struct SectionEffect
    {
        EffectType currentEffect = NONE;
        unsigned long lastUpdate = 0;
        unsigned long effectSpeed = DEFAULT_SPEED;
        // pulse effect parameters
        uint8_t pulse_r, pulse_g, pulse_b;
        uint16_t pulse_brightness = 0;
        int16_t pulse_step = 4;
        // streak effect parameters
        uint8_t streak_r, streak_g, streak_b;
        int streak_position = 0;
        uint8_t streak_trailLength = 2;
    };

    SectionEffect sections[NUM_SECTIONS];
    uint16_t sectionStarts[NUM_SECTIONS]; // array to store starting indices of each section
    uint8_t globalBrightness = DEFAULT_BRIGHTNESS;
    WS2812Serial leds;

    // utility functions for effects
    void applyBrightness(uint8_t r, uint8_t g, uint8_t b, uint8_t &r_out, uint8_t &g_out, uint8_t &b_out);
    void updatePulse(uint8_t section);
    void updateStreak(uint8_t section);

    // tracking previous positions for streak effect
    static int prev_positions[NUM_SECTIONS];

    __attribute__((aligned(32))) uint8_t drawingMemory[TOTAL_LEDS * 3];
    __attribute__((aligned(32))) DMAMEM uint8_t displayMemory[TOTAL_LEDS * 12];
};

#endif
