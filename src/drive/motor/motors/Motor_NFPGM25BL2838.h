/*
Motor_NFPGM25BL2838 - Specific implementation of motor template for this motor

Description:
Defines a concrete class that specializes generic motor template. Allows
specifying encoder typedef

Edit history:
5/9/25 - Created class - Aldem Pido

5/9/25 - [Rafeed Khan]
- cleaned up all the variable names to be more consistent (switched from camelCase to snake_case)
- simplified the logic inside SetSpeed() (especially that weird sign stuff) and made the direction calculation cleaner
- replaced all hardcoded timing values with static constexpr variables:
    - hard_reset_time_us = 103000ULL
    - nudge_threshold_us = 100000ULL
    - nudge_strength = 5.0f
- added noexcept to the constructor and SetSpeed() for safety and clarity
- deleted copy/move constructors and assignments to avoid accidental duplication
- renamed Base -> base, Timer -> timer, timeSinceReverse -> time_since_reverse for consistency
- replaced the old int sign logic with a float direction using a ternary operator
- made sure the encoder args get passed through cleanly with std::forward
- added static_assert to make sure timing thresholds are valid
- cleaned up the time-based control logic for easier reading: if a lot of time passed, reset;
  if some time passed, do a nudge; otherwise, apply speed normally

*/

#ifndef MOTOR_NFPGM25BL2838
#define MOTOR_NFPGM25BL2838

#include "MotorTemplate.h"
#include <cstdint>
#include <type_traits>
#include <utility>

// HELLO ALL!!!! ITS ME!!! RAFEED!!!!! I WILL BE WRITING THE COMMENTS FOR THIS FILE!!!!!!!
// ok so this class is a motor controller specifically for the NFP-GM25BL2838 motor
// it extends a generic Motor class and adds some timing-based logic to make the motor behave better when switching directions

template <typename EncoderType, typename... EncoderArgs>
class MOTOR_NFPGM25BL2838 : public Motor<RotationalMotor, EncoderType, RotationalMotorConstants, TwoSpeedDir> {
public:
    // alias for the base Motor class to make things easier to write
    using base = Motor<RotationalMotor, EncoderType, RotationalMotorConstants, TwoSpeedDir>;

    // timer type that keeps track of how long its been since we last did a "hard" direction change
    using timer = elapsedMicros;

private:
    timer time_since_reverse;
    
    // NOTE!!!! us means microseconds
    // how long (in microseconds) before we allow a full reset and apply the requested speed normally
    static constexpr std::uint64_t hard_reset_time_us = 103000ULL;

    // how long before we start "nudging" the motor with a tiny movement instead of applying full speed
    static constexpr std::uint64_t nudge_threshold_us = 100000ULL;

    // how strong the nudge is, in percent (5% of full speed)
    static constexpr float nudge_strength = 5.0f;

public:
    // constructor sets up the motor with constants, pin config, and optional encoder args
    // also resets the timer to zero when created
    MOTOR_NFPGM25BL2838(
        const RotationalMotorConstants& constants,
        const TwoSpeedDir& pin_config,
        EncoderArgs&&... encoder_args) noexcept(
            std::is_nothrow_constructible_v<base, const RotationalMotorConstants&, const TwoSpeedDir&, EncoderArgs&&...> &&
            std::is_nothrow_constructible_v<timer, int>
        )
        : base(constants, pin_config, std::forward<EncoderArgs>(encoder_args)...),
          time_since_reverse(0)
    {
        // sanity check: make sure nudge threshold is less than hard reset time
        static_assert(nudge_threshold_us < hard_reset_time_us);
    }

    // the core speed control method
    // this decides whether to apply the requested speed, give it a little nudge, or reset the timer
    void SetSpeed(float speed_value) noexcept {
        const std::uint64_t elapsed = static_cast<std::uint64_t>(time_since_reverse);

        if (elapsed > hard_reset_time_us) {
            // too much time has passed, so go ahead and apply the speed normally and reset the timer
            time_since_reverse = 0;
            base::SetSpeed(speed_value);
        } else if (elapsed > nudge_threshold_us) {
            // some time has passed, so don’t apply full speed, just nudge it in the right direction
            const float direction = (speed_value < 0.0f) ? -1.0f : 1.0f;
            base::SetSpeedPercentage(nudge_strength * direction);
        } else {
            // if it’s still too soon, just go ahead and apply the speed like normal
            base::SetSpeed(speed_value);
        }
    }

    // disable copy/move stuff since this object is tied to physical hardware and shouldn’t be duplicated or moved
    MOTOR_NFPGM25BL2838(const MOTOR_NFPGM25BL2838&) = delete;
    MOTOR_NFPGM25BL2838& operator=(const MOTOR_NFPGM25BL2838&) = delete;
    MOTOR_NFPGM25BL2838(MOTOR_NFPGM25BL2838&&) = delete;
    MOTOR_NFPGM25BL2838& operator=(MOTOR_NFPGM25BL2838&&) = delete;

    // default destructor — base class handles cleanup
    ~MOTOR_NFPGM25BL2838() = default;
};

#endif

