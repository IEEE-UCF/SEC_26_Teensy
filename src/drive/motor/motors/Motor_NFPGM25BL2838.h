/**
 * @file Motor_NFPGM25BL2838.h
 * @brief Specific implementation of motor template for the NFP-GM25BL2838 motor.
 *
 * Defines a concrete class that specializes the generic motor template.
 * Allows specifying encoder typedef.
 *
 * @authors Aldem Pido, Rafeed Khan
 * @section edit_history Edit history
 * - 5/9/25 - Created class - Aldem Pido
 * 5/9/25 - [Rafeed Khan]
 * - cleaned up all the variable names to be more consistent (switched from
 * camelCase to snake_case) - NOTE: The code currently uses camelCase for methods/members,
 * contrasting with this history entry.
 * - simplified the logic inside SetSpeed() (especially that weird sign stuff) and
 * made the direction calculation cleaner
 * - replaced all hardcoded timing values with static constexpr variables:
 * - hard_reset_time_us = 103000ULL
 * - nudge_threshold_us = 100000ULL
 * - nudge_strength = 5.0f
 * - added noexcept to the constructor and SetSpeed() for safety and clarity
 * - deleted copy/move constructors and assignments to avoid accidental duplication
 * - renamed Base -> base, Timer -> timer, timeSinceReverse -> time_since_reverse
 * for consistency
 * - replaced the old int sign logic with a float direction using a ternary
 * operator
 * - made sure the encoder args get passed through cleanly with std::forward
 * - added static_assert to make sure timing thresholds are valid
 * - cleaned up the time-based control logic for easier reading: if a lot of time
 * passed, reset; if some time passed, do a nudge; otherwise, apply speed normally
 * - 5/9/25 - Moved removal of default constructors to main template
 */

#ifndef MOTOR_NFPGM25BL2838
#define MOTOR_NFPGM25BL2838

#include <elapsedMillis.h>

#include <cstdint>
#include <type_traits>
#include <utility>

#include "MotorTemplate.h"

// HELLO ALL!!!! ITS ME!!! RAFEED!!!!! I WILL BE WRITING THE COMMENTS FOR THIS
// FILE!!!!!!! ok so this class is a motor controller specifically for the
// NFP-GM25BL2838 motor it extends a generic Motor class and adds some
// timing-based logic to make the motor behave better when switching directions

/**
 * @ingroup motor_template
 * @brief Specific concrete motor class for the NFP-GM25BL2838 motor.
 *
 * This class inherits from the generic `Motor` template, specializing it
 * for rotational motors (`RotationalMotor`) using constants for rotational
 * motors (`RotationalMotorConstants`) and the `TwoSpeedDir` pin configuration
 * specific to the NFP-GM25BL2838 motor driver. It adds custom timing-based logic
 * in the `SetSpeed()` method to improve behavior during direction changes.
 *
 * @tparam EncoderType The type of encoder used with this motor instance.
 * @tparam EncoderArgs Variadic template parameter pack to capture arguments
 * needed for the `EncoderType` constructor.
 */
template <typename EncoderType, typename... EncoderArgs>
class Motor_NFPGM25BL2838
    : public Motor<RotationalMotor, EncoderType, RotationalMotorConstants, TwoSpeedDir> {
 public:
  /**
   * @brief Type alias for the specialized base class.
   * For convenience when referring to the base type.
   */
  using base = Motor<RotationalMotor, EncoderType, RotationalMotorConstants, TwoSpeedDir>;
  using timer = elapsedMillis;

  /**
   * @brief Constructs a Motor_NFPGM25BL2838 instance.
   *
   * Sets up the motor with provided constants and pin configuration,
   * forwards optional encoder arguments to the base class constructor,
   * and initializes the internal direction change timer.
   *
   * @param constants Rotational motor constants.
   * @param pin_config TwoSpeedDir pin configuration specific to this motor driver.
   * @param encoder_args Variadic arguments for the `EncoderType` constructor.
   */
  Motor_NFPGM25BL2838(
      const RotationalMotorConstants& constants, const TwoSpeedDir& pin_config,
      EncoderArgs&&... encoder_args) noexcept(std::
                                                  is_nothrow_constructible_v<
                                                      base, const RotationalMotorConstants&,
                                                      const TwoSpeedDir&, EncoderArgs&&...> &&
                                              std::is_nothrow_constructible_v<timer, int>)
      : base(constants, pin_config, std::forward<EncoderArgs>(encoder_args)...),
        time_since_reverse(0) {
    // sanity check: make sure nudge threshold is less than hard reset time
    static_assert(nudge_threshold_us < hard_reset_time_us);
  }

  /**
   * @brief Default destructor.
   */
  ~Motor_NFPGM25BL2838() = default;

  /**
   * Sets motor speed. Contains logic for NFPShop25dBrushless weird edge case where the motor won't
   * start sometimes. This is fixed by nudging the motor in the opposite direction.
   * @param speed_value the speed value. Input based on max speed input
   */
  void SetSpeed(float speed_value) noexcept {
    // nudges motor in opposite direction based on a timer
    const std::uint64_t elapsed = static_cast<std::uint64_t>(time_since_reverse);
    if (elapsed > hard_reset_time_us) {
      time_since_reverse = 0;
      base::SetSpeed(speed_value);
    } else if (elapsed > nudge_threshold_us) {
      const float direction = (speed_value < 0.0f) ? -1.0f : 1.0f;
      base::SetSpeedPercentage(nudge_strength * direction);
    } else {
      base::SetSpeed(speed_value);
    }
  }

 private:
  timer time_since_reverse;

  // NOTE!!!! us stands for microseconds
  static constexpr std::uint64_t hard_reset_time_us = 103000ULL;  // time to end nudge
  static constexpr std::uint64_t nudge_threshold_us = 100000ULL;  // time to start nudge
  static constexpr float nudge_strength = 5.0f;                   // expressed as percentage
};

#endif
