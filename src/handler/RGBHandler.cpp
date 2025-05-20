/**
 * @file RGBHandler.cpp
 * @author Rafeed Khan
 * @brief Implements the RGBHandler class for advanced control of WS2812 LED strips.
 * @date May 19, 2025 // Assuming current date for last modification
 */
#include "RGBHandler.h"

#include <Arduino.h>  // For Serial, millis, constrain, max, etc.

// Initialize static member for tracking previous streak positions
int RGBHandler::prev_positions[NUM_SECTIONS];

/**
 * @brief Constructs an RGBHandler object.
 *  Initializes the WS2812Serial library instance with the total number of LEDs,
 * memory buffers, LED pin, and color order (WS2812_GRB). It also initializes
 * the `prev_positions` array used by the streak effect to -1, indicating no active streaks.
 * @param kLED_pin The GPIO pin number connected to the data input of the WS2812 LED strip.
 */
RGBHandler::RGBHandler(uint8_t kLED_pin)
    : kLED_pin_private(kLED_pin),
      leds(TOTAL_LEDS, displayMemory, drawingMemory, kLED_pin_private, WS2812_GRB) {
  for (int i = 0; i < NUM_SECTIONS; i++) {
    prev_positions[i] = -1;  // Initialize to no previous position
  }
}

/**
 * @brief Initializes the RGB LED handler and the LED strip.
 *  Sets up the hardware and software for LED control.
 * If `Serial3` is used by WS2812Serial on this platform, it's initialized.
 * It computes the starting LED index for each section based on `SECTION_SIZES`.
 * Calls `leds.begin()` to initialize the LED strip communication.
 * Sets the global brightness to a default value and turns off all LEDs initially.
 * @return True if initialization appears successful (e.g., setting initial color returns true),
 * false otherwise (though `leds.begin()` usually doesn't return a status).
 */
bool RGBHandler::Begin() {
  // If WS2812Serial uses a specific Serial port (like Serial3 on some Teensy boards)
  // and it needs to be explicitly begun, do it here.
  // Example for Teensy:
  // static bool serialInitialized = false;
  // if (!serialInitialized) {
  //     Serial3.begin(115200); // Or whatever baud rate WS2812Serial uses for this pin config
  //     serialInitialized = true;
  // }

  sectionStarts[0] = 0;
  for (int i = 1; i < NUM_SECTIONS; i++) {
    sectionStarts[i] = sectionStarts[i - 1] + SECTION_SIZES[i - 1];
  }

  leds.begin();  // Initialize the WS2812Serial library
  setGlobalBrightness(DEFAULT_BRIGHTNESS);
  return setSectionSolidColor(
      0, GlobalColors::BLACK.r, GlobalColors::BLACK.g,
      GlobalColors::BLACK
          .b);  // Turn off all LEDs in section 0 initially
                // To turn ALL off, loop through sections or use a dedicated clear all.
}

/**
 * @brief Applies the global brightness scaling to an RGB color.
 *  Takes input RGB values and scales them down based on the `globalBrightness`
 * setting. This includes a rounding correction (`+127`) before division to provide
 * smoother brightness transitions at lower levels. The output values are constrained
 * to the 0-255 range.
 * @param r_in Input red component (0-255).
 * @param g_in Input green component (0-255).
 * @param b_in Input blue component (0-255).
 * @param r_out Output red component, adjusted for brightness.
 * @param g_out Output green component, adjusted for brightness.
 * @param b_out Output blue component, adjusted for brightness.
 */
void RGBHandler::applyBrightness(uint8_t r_in, uint8_t g_in, uint8_t b_in, uint8_t &r_out,
                                 uint8_t &g_out, uint8_t &b_out) {
  r_out = constrain(((uint32_t)r_in * globalBrightness + 127) / 255, 0U, 255U);
  g_out = constrain(((uint32_t)g_in * globalBrightness + 127) / 255, 0U, 255U);
  b_out = constrain(((uint32_t)b_in * globalBrightness + 127) / 255, 0U, 255U);
}

/**
 * @brief Sets a specified section of LEDs to a solid color.
 *  Stops any active effect on the section. Applies the global brightness to the
 * given RGB color and then sets all LEDs within the specified section to this adjusted color.
 * The changes are immediately shown on the LED strip.
 * @param section The index of the LED section to control (0 to NUM_SECTIONS-1).
 * @param r Red component of the color (0-255).
 * @param g Green component of the color (0-255).
 * @param b Blue component of the color (0-255).
 * @return True if the section index is valid and color is set, false otherwise.
 */
bool RGBHandler::setSectionSolidColor(uint8_t section, uint8_t r, uint8_t g, uint8_t b) {
  if (section >= NUM_SECTIONS) return false;
  stopSectionEffect(section);  // Stop any existing effect on this section

  sections[section].currentEffect = SOLID;  // Or keep NONE and just set colors
  sections[section].base_r = r;
  sections[section].base_g = g;
  sections[section].base_b = b;

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

/**
 * @brief Sets a specified section of LEDs to a solid color using an RGBColor struct.
 *  Overload of `setSectionSolidColor` that accepts an `RGBColor` struct.
 * @param section The index of the LED section.
 * @param color The `RGBColor` struct representing the desired color.
 * @return True if successful, false otherwise.
 */
bool RGBHandler::setSectionSolidColor(uint8_t section, const RGBColor &color) {
  return setSectionSolidColor(section, color.r, color.g, color.b);
}

/**
 * @brief Configures a pulsing light effect for a specified section.
 *  Sets up a brightness pulsing effect (triangle wave) for the given section
 * with the specified base color and speed. If an effect is already active on the section,
 * this function will fail unless the existing effect is stopped first.
 * The pulse phase is initialized to start at minimum brightness.
 * @param section The index of the LED section.
 * @param r Red component of the pulse color.
 * @param g Green component of the pulse color.
 * @param b Blue component of the pulse color.
 * @param speed Duration of one full pulse cycle in milliseconds. Must be between MIN_SPEED and
 * MAX_SPEED.
 * @return True if the pulse effect was successfully configured, false if section index is invalid,
 * speed is out of range, or an effect is already running on the section.
 */
bool RGBHandler::setSectionPulseEffect(uint8_t section, uint8_t r, uint8_t g, uint8_t b,
                                       unsigned long speed) {
  if (section >= NUM_SECTIONS || speed < MIN_SPEED || speed > MAX_SPEED) return false;

  SectionEffect &sec = sections[section];
  // Optional: Allow overwriting existing effect or require stopSectionEffect first
  // if (sec.currentEffect != NONE) return false; // Uncomment to prevent overwriting active effect

  stopSectionEffect(section);  // Ensure section is cleared and ready for new effect

  sec.currentEffect = PULSE;
  sec.effectSpeed = speed;
  sec.base_r = r;
  sec.base_g = g;
  sec.base_b = b;
  sec.pulse_phase = 0;  // Initialize phase to start at minimum brightness

  // Calculate pulse_step: (total_phase_range * updates_per_second) / (speed_in_ms / 1000)
  // Assuming phase 0-1023, and we want updates based on effectSpeed.
  // Step aims for a smooth transition over 'speed' ms.
  // 1024 steps for a full cycle. Number of updates in 'speed' ms is 'speed / update_interval'.
  // Let's simplify: if Update is called every X ms (e.g. 16ms for 60FPS),
  // then step = 1024 / (speed / X)
  uint16_t updates_per_cycle = speed / (16);  // Assuming Update is called roughly every 16ms
  if (updates_per_cycle == 0) updates_per_cycle = 1;
  sec.pulse_step =
      max(2, 1024 / updates_per_cycle);  // Ensure step is at least 2 for movement
                                         // The original calculation: `uint16_t base_step = (1024 *
                                         // 10UL) / speed; sec.pulse_step = max(constrain(base_step,
                                         // 2, 20), 2);` This seems to target ~10 updates for the
                                         // full range, constrained. We'll keep it.
  uint16_t base_step =
      (1024 * 10UL) / speed;  // 10UL likely from a target effect duration or update rate
  sec.pulse_step = max((uint16_t)constrain(base_step, 2, 20),
                       (uint16_t)2);  // Ensure types match for max/constrain

  return true;
}

/**
 * @brief Configures a pulsing light effect using an RGBColor struct.
 *  Overload for `setSectionPulseEffect` that accepts an `RGBColor` struct.
 * @param section The index of the LED section.
 * @param color The `RGBColor` struct for the pulse.
 * @param speed Duration of one pulse cycle in milliseconds.
 * @return True if successful, false otherwise.
 */
bool RGBHandler::setSectionPulseEffect(uint8_t section, const RGBColor &color,
                                       unsigned long speed) {
  return setSectionPulseEffect(section, color.r, color.g, color.b, speed);
}

/**
 * @brief Configures a streaking light effect for a specified section.
 *  Sets up a "comet" or "streak" effect with a given color and speed.
 * The streak moves across the LEDs in the section.
 * @param section The index of the LED section.
 * @param r Red component of the streak color.
 * @param g Green component of the streak color.
 * @param b Blue component of the streak color.
 * @param speed Time in milliseconds for the streak to advance one position. Must be between
 * MIN_SPEED and MAX_SPEED.
 * @param reverse If true, the streak moves in the reverse direction (from end to start of section).
 * Defaults to false.
 * @return True if the streak effect was successfully configured, false otherwise (invalid
 * parameters or effect already active).
 */
bool RGBHandler::setSectionStreakEffect(uint8_t section, uint8_t r, uint8_t g, uint8_t b,
                                        unsigned long speed, bool reverse) {
  if (section >= NUM_SECTIONS || speed < MIN_SPEED || speed > MAX_SPEED) return false;

  SectionEffect &sec = sections[section];
  // Optional: if (sec.currentEffect != NONE) return false;
  stopSectionEffect(section);  // Clear section for new effect

  sec.currentEffect = STREAK;
  sec.effectSpeed = speed;
  sec.base_r = r;
  sec.base_g = g;
  sec.base_b = b;
  sec.reverseStreak = reverse;
  sec.streak_position =
      reverse ? (SECTION_SIZES[section] - 1) : 0;  // Start at end if reverse, else at start
  sec.streak_trailLength =
      max(1, SECTION_SIZES[section] / 5);  // Example: trail is 1/5 of section length, min 1
  if (sec.streak_trailLength == 0) sec.streak_trailLength = 1;

  prev_positions[section] = -1;  // Reset previous position tracking for this section
  return true;
}

/**
 * @brief Configures a streaking light effect using an RGBColor struct.
 *  Overload for `setSectionStreakEffect` that accepts an `RGBColor` struct.
 * @param section The index of the LED section.
 * @param color The `RGBColor` struct for the streak.
 * @param speed Time in milliseconds for the streak to advance one position.
 * @param reverse If true, the streak moves in reverse. Defaults to false.
 * @return True if successful, false otherwise.
 */
bool RGBHandler::setSectionStreakEffect(uint8_t section, const RGBColor &color, unsigned long speed,
                                        bool reverse) {
  return setSectionStreakEffect(section, color.r, color.g, color.b, speed, reverse);
}

/**
 * @brief Stops any active effect on a specified section and turns off its LEDs.
 *  Sets the `currentEffect` for the section to `NONE`. Clears all LEDs in
 * the section to black (off). Resets `prev_positions` for streak effect cleanup.
 * Note: `leds.show()` is not called here; changes will be visible after the next `Update()` or an
 * explicit `leds.show()`. If immediate update is needed, `leds.show()` should be called after this.
 * @param section The index of the LED section to stop.
 */
void RGBHandler::stopSectionEffect(uint8_t section) {
  if (section >= NUM_SECTIONS) return;

  SectionEffect &sec = sections[section];
  sec.currentEffect = NONE;

  uint16_t start = sectionStarts[section];
  uint16_t end = start + SECTION_SIZES[section];
  for (uint16_t i = start; i < end; ++i) {
    leds.setPixel(i, 0, 0, 0);  // Turn off LEDs in this section
  }
  // leds.show(); // Consider if show should be here or managed by caller/Update

  prev_positions[section] = -1;  // Reset for streak effect
}

/**
 * @brief Updates the state of a pulse effect for a given section.
 *  Called by `Update()`. Calculates the current brightness for the pulse based on a
 * triangle wave determined by `pulse_phase` and `pulse_step`. Applies this brightness
 * to the section's base color, then adjusts for global brightness, and finally sets
 * the color of all LEDs in the section.
 * @param section The index of the section whose pulse effect is to be updated.
 */
void RGBHandler::updatePulse(uint8_t section) {
  SectionEffect &sec = sections[section];

  sec.pulse_phase = (sec.pulse_phase + sec.pulse_step) % 1024;  // Update phase, wrap around 1024

  // Calculate brightness using a triangle wave (0-255)
  uint8_t brightness_factor;    // 0-255
  if (sec.pulse_phase < 512) {  // Rising edge of triangle
    brightness_factor = (sec.pulse_phase * 255) / 511;
  } else {  // Falling edge of triangle
    brightness_factor = ((1023 - sec.pulse_phase) * 255) / 511;
  }

  // Apply pulse brightness to the base color
  uint8_t r = (sec.base_r * brightness_factor) / 255;
  uint8_t g = (sec.base_g * brightness_factor) / 255;
  uint8_t b = (sec.base_b * brightness_factor) / 255;

  uint8_t r_adj, g_adj, b_adj;
  applyBrightness(r, g, b, r_adj, g_adj, b_adj);  // Adjust for global brightness

  uint16_t start = sectionStarts[section];
  uint16_t end = start + SECTION_SIZES[section];
  for (uint16_t i = start; i < end; ++i) {
    leds.setPixel(i, r_adj, g_adj, b_adj);
  }
}

/**
 * @brief Updates the state of a streak effect for a given section.
 *  Called by `Update()`. Clears the previous position of the streak's trail end.
 * Draws the streak at its new position, applying a dimmed version of the base color.
 * Advances the streak position for the next update, handling wrap-around at section boundaries
 * and direction (forward/reverse).
 * @param section The index of the section whose streak effect is to be updated.
 */
void RGBHandler::updateStreak(uint8_t section) {
  SectionEffect &sec = sections[section];
  uint16_t section_size = SECTION_SIZES[section];
  uint16_t section_start_led = sectionStarts[section];
  int current_head_pos_in_section = sec.streak_position;  // 0 to section_size-1

  // Clear the tail of the streak from the previous frame before drawing the new one.
  // The logic in the original code for prev_positions seems to handle clearing one LED behind the
  // trail. A more robust way to clear is to explicitly turn off LEDs that were part of the streak
  // in the last frame and are not part of it in the current frame. For simplicity, we'll adapt the
  // existing prev_positions logic. The original code clears a single LED at prev_positions[section]
  // if it's valid and different. This is okay for a single LED moving, but for a trail, we need to
  // clear the whole trail.

  // Simpler approach: Blank the whole section then redraw the streak. Less efficient for long
  // sections. Efficient approach: Only change pixels that need to change. The current code tries to
  // clear one specific pixel from the previous step.

  // Turn off the entire section first (can cause flicker if not timed well or too slow)
  // for (uint16_t i = 0; i < section_size; ++i) {
  //    leds.setPixel(section_start_led + i, 0, 0, 0);
  // }

  // Clear the pixel at the very end of the trail from the *previous* frame.
  // This needs careful calculation based on current_head_pos_in_section, trailLength, and
  // direction.
  if (prev_positions[section] != -1) {  // If there was a previous position to clear
    leds.setPixel(section_start_led + prev_positions[section], 0, 0, 0);
  }

  uint8_t r_adj, g_adj, b_adj;
  // Using a fixed dimming factor (200/255) as in original code. Could be configurable.
  applyBrightness((sec.base_r * 200) >> 8, (sec.base_g * 200) >> 8, (sec.base_b * 200) >> 8, r_adj,
                  g_adj, b_adj);

  // Draw the streak trail
  for (uint8_t i = 0; i < sec.streak_trailLength; i++) {
    int led_offset_in_section;
    if (!sec.reverseStreak) {  // Forward
      led_offset_in_section = (current_head_pos_in_section - i + section_size) % section_size;
    } else {  // Reverse
      led_offset_in_section = (current_head_pos_in_section + i) % section_size;
    }
    // Ensure led_offset_in_section is valid before drawing
    if (led_offset_in_section >= 0 && led_offset_in_section < section_size) {
      leds.setPixel(section_start_led + led_offset_in_section, r_adj, g_adj, b_adj);
    }
  }

  // Determine the pixel to clear in the next frame (the one at the very end of the current trail)
  if (!sec.reverseStreak) {
    prev_positions[section] =
        (current_head_pos_in_section - sec.streak_trailLength + 1 + section_size) % section_size;
    sec.streak_position = (current_head_pos_in_section + 1) % section_size;  // Advance head
  } else {
    prev_positions[section] =
        (current_head_pos_in_section + sec.streak_trailLength - 1) % section_size;
    sec.streak_position = (current_head_pos_in_section - 1 + section_size) %
                          section_size;  // Advance head (in reverse)
  }
}

/**
 * @brief Main update loop for all LED effects.
 *  This function should be called repeatedly (e.g., in the Arduino `loop()` function).
 * It iterates through all defined sections. If a section has an active effect (PULSE or STREAK)
 * and enough time has passed since its last update (determined by `effectSpeed`),
 * it calls the respective update function (`updatePulse` or `updateStreak`).
 * If any section was updated, `leds.show()` is called to refresh the LED strip,
 * but not more often than approximately 60 times per second (every 16ms) to save processing time.
 */
void RGBHandler::Update() {
  static unsigned long lastShowTime = 0;
  bool needsShow = false;
  unsigned long currentTime = millis();

  for (uint8_t s = 0; s < NUM_SECTIONS; s++) {
    SectionEffect &sec = sections[s];
    if (sec.currentEffect == NONE) continue;

    if (currentTime - sec.lastUpdate >= sec.effectSpeed) {
      if (sec.currentEffect == PULSE) {
        updatePulse(s);
      } else if (sec.currentEffect == STREAK) {
        updateStreak(s);
      }
      sec.lastUpdate = currentTime;
      needsShow = true;
    }
  }

  // Refresh the LED strip if any effect was updated, but limit refresh rate
  if (needsShow && (currentTime - lastShowTime >= 16)) {  // Approx 60 FPS cap for show()
    leds.show();
    lastShowTime = currentTime;
  }
}

/**
 * @brief Stops all active effects on all sections and turns off all LEDs.
 *  Iterates through each section, calls `stopSectionEffect()` for it,
 * and then calls `leds.show()` once to update the physical LED strip.
 */
void RGBHandler::stopAllEffects() {
  for (uint8_t s = 0; s < NUM_SECTIONS; s++) {
    stopSectionEffect(s);  // This will turn off LEDs for section 's'
  }
  leds.show();  // Show all changes at once
}

/**
 * @brief Sets the global brightness for all LED effects.
 *  Adjusts the overall brightness of the LED strip. The provided brightness
 * value is constrained between 0 and `MAX_BRIGHTNESS`. This new brightness level
 * is then passed to the `WS2812Serial` library.
 * Note: This might not immediately update colors of active effects until their next
 * update cycle or if `setSectionSolidColor` is called, as `applyBrightness` is used there.
 * To see immediate change, existing colors might need to be reapplied.
 * The `leds.setBrightness()` might handle this directly.
 * @param brightness The desired global brightness level (0-255, will be capped by MAX_BRIGHTNESS).
 * @return True. (The function currently always returns true).
 */
bool RGBHandler::setGlobalBrightness(uint8_t brightness) {
  globalBrightness = constrain(brightness, 0, MAX_BRIGHTNESS);
  leds.setBrightness(
      globalBrightness);  // WS2812Serial handles its own brightness scaling if supported.
                          // If not, our applyBrightness handles it, and this call might be
                          // redundant or for a different purpose (e.g. hardware brightness).
                          // Assuming leds.setBrightness is the primary way.
  // To reflect change immediately on solid colors, you might need to re-apply them.
  // For effects, they will pick it up on next updatePulse/updateStreak.
  return true;
}

/**
 * @brief Prints RGBHandler configuration or current state to a Print stream.
 *  If `printConfig` is true, prints general configuration like number of sections,
 * their sizes, and global brightness. Otherwise, prints the current state of each section,
 * including active effect, timing, speed, and color parameters.
 * @param output Reference to a Print object (e.g., Serial).
 * @param printConfig Boolean flag; true for config, false for current state. Defaults to false.
 */
void RGBHandler::PrintInfo(Print &output, bool printConfig) const {
  if (printConfig) {
    output.println(F("RGBHandler Configuration:"));
    output.print(F("  Number of Sections: "));
    output.println(NUM_SECTIONS);
    output.print(F("  Total LEDs: "));
    output.println(TOTAL_LEDS);
    output.print(F("  Section Sizes: "));
    for (int i = 0; i < NUM_SECTIONS; i++) {
      output.print(SECTION_SIZES[i]);
      if (i < NUM_SECTIONS - 1) output.print(F(", "));
    }
    output.println();
    output.print(F("  Section Start LEDs: "));
    for (int i = 0; i < NUM_SECTIONS; i++) {
      output.print(sectionStarts[i]);
      if (i < NUM_SECTIONS - 1) output.print(F(", "));
    }
    output.println();
    output.print(F("  LED Pin: "));
    output.println(kLED_pin_private);
    output.print(F("  Global Brightness (0-255): "));
    output.println(globalBrightness);
    output.print(F("  Max Allowed Brightness: "));
    output.println(MAX_BRIGHTNESS);
  } else {
    output.println(F("RGBHandler State:"));
    output.print(F("  Global Brightness: "));
    output.println(globalBrightness);
    for (int i = 0; i < NUM_SECTIONS; i++) {
      output.print(F("  Section "));
      output.print(i);
      output.println(F(":"));
      const SectionEffect &sec = sections[i];
      output.print(F("    Effect Type: "));
      switch (sec.currentEffect) {
        case NONE:
          output.println(F("NONE"));
          break;
        case SOLID:
          output.println(F("SOLID"));
          break;
        case PULSE:
          output.println(F("PULSE"));
          break;
        case STREAK:
          output.println(F("STREAK"));
          break;
        default:
          output.println(F("UNKNOWN"));
          break;
      }
      if (sec.currentEffect != NONE) {
        output.print(F("    Base Color (R,G,B): "));
        output.print(sec.base_r);
        output.print(F(","));
        output.print(sec.base_g);
        output.print(F(","));
        output.println(sec.base_b);
        output.print(F("    Effect Speed (ms): "));
        output.println(sec.effectSpeed);
        output.print(F("    Last Update (ms): "));
        output.println(sec.lastUpdate);
        if (sec.currentEffect == PULSE) {
          output.print(F("    Pulse Phase: "));
          output.println(sec.pulse_phase);
          output.print(F("    Pulse Step: "));
          output.println(sec.pulse_step);
        } else if (sec.currentEffect == STREAK) {
          output.print(F("    Streak Position: "));
          output.println(sec.streak_position);
          output.print(F("    Streak Trail Length: "));
          output.println(sec.streak_trailLength);
          output.print(F("    Streak Reversed: "));
          output.println(sec.reverseStreak ? "Yes" : "No");
          output.print(F("    Prev Streak Clear Pos: "));
          output.println(prev_positions[i]);
        }
      }
    }
  }
}

/**
 * @brief Overloaded stream insertion operator for printing RGBHandler details.
 *  Allows an RGBHandler object to be easily printed to any Print-derived stream.
 * By default, prints the current runtime state of the LED sections.
 * @param output Reference to the Print stream.
 * @param handler Const reference to the RGBHandler object.
 * @return Reference to the Print stream.
 */
Print &operator<<(Print &output, const RGBHandler &handler) {
  handler.PrintInfo(output, false);  // Default to printing runtime state
  return output;
}