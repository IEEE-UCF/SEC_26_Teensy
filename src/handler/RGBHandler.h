/**
 * @file RGBHandler.h
 * @author Rafeed Khan
 * @brief Defines the RGBHandler class for controlling RGB LED strips divided into sections.
 * @date May 19, 2025 // Assuming current date for last modification
 * @defgroup lighting RGB LED System
 * This group contains components for managing RGB LED effects (solid, pulse, streak)
 * and global brightness control over multiple LED sections.
 */

#ifndef RGBHANDLER_H
#define RGBHANDLER_H

#include <Arduino.h>
#include <WS2812Serial.h>  // Library for controlling WS2812 LEDs

// Forward declaration if EffectType is an enum defined elsewhere or define it here
enum EffectType {
  NONE,
  PULSE,
  STREAK,
  SOLID
};  // Assuming SOLID might be implicitly handled or needed

// Constants that might be defined in a config file or here for compilation
#ifndef NUM_SECTIONS
#define NUM_SECTIONS 3  // Example: Number of LED sections
#endif
#ifndef TOTAL_LEDS
#define TOTAL_LEDS 60  // Example: Total number of LEDs in the strip
#endif
// Example SECTION_SIZES, should match NUM_SECTIONS
const uint16_t SECTION_SIZES[NUM_SECTIONS] = {20, 20, 20};  // Example sizes

#define DEFAULT_BRIGHTNESS 50  ///< Default global brightness level (0-255, but often capped lower).
#define MAX_BRIGHTNESS 150     ///< Maximum allowable global brightness.
#define DEFAULT_SPEED 100      ///< Default speed for effects (milliseconds).
#define MIN_SPEED 10           ///< Minimum speed for effects (fastest).
#define MAX_SPEED 1000         ///< Maximum speed for effects (slowest).

/**
 * @struct RGBColor
 * @brief Represents an RGB color.
 * @ingroup lighting
 *  Stores red, green, and blue components of a color, each as an 8-bit unsigned integer.
 */
struct RGBColor {
  uint8_t r;  ///< Red component (0-255).
  uint8_t g;  ///< Green component (0-255).
  uint8_t b;  ///< Blue component (0-255).
};

/**
 * @namespace GlobalColors
 * @ingroup lighting
 * @brief Defines common RGB color presets.
 *  Provides a set of predefined RGBColor constants for easy use.
 */
namespace GlobalColors {
constexpr RGBColor GOLD = {255, 180, 0};     ///< Gold color preset.
constexpr RGBColor PURPLE = {170, 0, 255};   ///< Purple color preset.
constexpr RGBColor GREEN = {0, 255, 0};      ///< Green color preset.
constexpr RGBColor CYAN = {0, 255, 255};     ///< Cyan color preset.
constexpr RGBColor RED = {255, 0, 0};        ///< Red color preset.
constexpr RGBColor BLUE = {0, 0, 255};       ///< Blue color preset.
constexpr RGBColor BLACK = {0, 0, 0};        ///< Black color preset (off).
constexpr RGBColor WHITE = {255, 255, 255};  ///< White color preset.
}  // namespace GlobalColors

/**
 * @class RGBHandler
 * @ingroup lighting
 * @brief Manages RGB LED strip effects across multiple sections.
 *  This class handles initialization, updates, and control of WS2812-based LED strips.
 * It allows dividing the strip into logical sections, each capable of displaying independent
 * effects like solid color, pulsing, or streaking. It also supports global brightness control.
 */
class RGBHandler {
 public:
  RGBHandler(uint8_t kLED_pin);  // Renamed parameter for clarity
  bool Begin();
  void Update();

  // Solid color effects
  bool setSectionSolidColor(uint8_t section, uint8_t r, uint8_t g, uint8_t b);
  bool setSectionSolidColor(uint8_t section, const RGBColor &color);

  // Pulse effects
  bool setSectionPulseEffect(uint8_t section, uint8_t r, uint8_t g, uint8_t b, unsigned long speed);
  bool setSectionPulseEffect(uint8_t section, const RGBColor &color, unsigned long speed);

  // Streak effects (including reverse)
  bool setSectionStreakEffect(uint8_t section, uint8_t r, uint8_t g, uint8_t b, unsigned long speed,
                              bool reverse = false);
  bool setSectionStreakEffect(uint8_t section, const RGBColor &color, unsigned long speed,
                              bool reverse = false);

  // bool processCommand(const String &command); // If implemented, needs definition
  void stopSectionEffect(uint8_t section);
  void stopAllEffects();
  bool setGlobalBrightness(uint8_t brightness);

  void PrintInfo(Print &output, bool printConfig = false) const;
  friend Print &operator<<(Print &output, const RGBHandler &handler);

 private:
  uint8_t kLED_pin_private;  ///< GPIO pin connected to the LED strip's data line.

  /**
   * @struct SectionEffect
   * @brief Stores the state and parameters for an LED effect on a specific section.
   *  Internal structure to manage the active effect, timing, color, and
   * other effect-specific parameters for one LED section.
   */
  struct SectionEffect {
    EffectType currentEffect = NONE;            ///< Type of the currently active effect.
    unsigned long lastUpdate = 0;               ///< Timestamp of the last effect update.
    unsigned long effectSpeed = DEFAULT_SPEED;  ///< Speed of the current effect in ms.
    uint8_t base_r, base_g, base_b;             ///< Base color for the effect.
    uint16_t pulse_phase = 0;                   ///< Current phase for pulse effect calculation.
    uint16_t pulse_step = 0;                    ///< Step size for pulse phase increment.
    int streak_position = 0;                    ///< Current leading position of the streak.
    uint8_t streak_trailLength = 2;             ///< Length of the streak trail.
    bool reverseStreak = false;                 ///< Direction of the streak effect.
  };

  SectionEffect sections[NUM_SECTIONS];           ///< Array of effect states for each section.
  uint16_t sectionStarts[NUM_SECTIONS];           ///< Starting LED index for each section.
  uint8_t globalBrightness = DEFAULT_BRIGHTNESS;  ///< Current global brightness level.

  // WS2812Serial requires DMA buffers. These must be sized correctly.
  // TOTAL_LEDS * 3 for GRB, TOTAL_LEDS * 12 for internal display buffer (check lib specifics)
  uint8_t drawingMemory[TOTAL_LEDS * 3];
  alignas(4) uint8_t
      displayMemory[TOTAL_LEDS * 12];  // WS2812Serial often uses 12 bytes per LED for its display
                                       // buffer. `alignas(4)` or `aligned(4)` for DMA.

  WS2812Serial leds;  ///< WS2812Serial library instance.

  static int prev_positions[NUM_SECTIONS];  ///< Stores previous streak positions for clearing.

  void applyBrightness(uint8_t r_in, uint8_t g_in, uint8_t b_in, uint8_t &r_out, uint8_t &g_out,
                       uint8_t &b_out);
  void updatePulse(uint8_t section);
  void updateStreak(uint8_t section);
};

#endif  // RGBHANDLER_H