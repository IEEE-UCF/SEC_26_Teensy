/**
 * @file Motor_GenericHBridge.h
 * @brief Example implementation for a Rotational H-Bridge Motor.
 *
 * Defines a concrete class that specializes the generic motor template.
 * Allows specifying encoder typedef.
 *
 * @authors Aldem Pido
 * @section edit_history Edit history
 * - 5/9/25 - Created class - Aldem Pido
 * - 5/9/25 - Moved protection deletion constructors to main template
 */
#ifndef MOTOR_GENERICHBRIDGE
#define MOTOR_GENERICHBRIDGE

#include <type_traits>  // For std::is_nothrow_constructible_v
#include <utility>      // For std::forward

#include "MotorTemplate.h"

/**
 * @ingroup motor_template
 * @brief Example concrete motor class for a generic H-bridge controlled rotational motor.
 *
 * This class inherits from the generic `Motor` template, fixing the `MotorType`
 * to `RotationalMotor`, the `ConstantsType` to `RotationalMotorConstants`, and
 * the `PinConfigType` to `ThreeSpeedDirDir`. It remains templated on the
 * `EncoderType`, allowing flexibility in choosing the encoder.
 *
 * @tparam EncoderType The type of encoder used with this motor instance.
 * @tparam EncoderArgs Variadic template parameter pack to capture arguments
 * needed for the `EncoderType` constructor.
 */
template <typename EncoderType, typename... EncoderArgs>
class Motor_GenericHBridge
    : public Motor<RotationalMotor, EncoderType, RotationalMotorConstants, ThreeSpeedDirDir> {
 public:
  /**
   * @brief Type alias for the specialized base class.
   * For convenience when referring to the base type.
   */
  using base = Motor<RotationalMotor, EncoderType, RotationalMotorConstants, ThreeSpeedDirDir>;

  /**
   * @brief Constructs a Motor_GenericHBridge instance.
   *
   * Initializes the base `Motor` class with the provided constants, pin configuration,
   * and forwards any additional arguments to the encoder constructor (if applicable).
   *
   * @param constants Rotational motor constants.
   * @param pin_config ThreeSpeedDirDir pin configuration.
   * @param encoder_args Variadic arguments for the `EncoderType` constructor.
   */
  Motor_GenericHBridge(
      const RotationalMotorConstants &constants, const ThreeSpeedDirDir &pin_config,
      EncoderArgs &&...encoder_args) noexcept(std::
                                                  // Check if the base class is nothrow
                                                  // constructible from the arguments
                                              is_nothrow_constructible_v<
                                                  base, const RotationalMotorConstants &,
                                                  const ThreeSpeedDirDir &,
                                                  EncoderArgs &&...>)  // Corrected trait args
      : base(constants, pin_config, std::forward<EncoderArgs>(encoder_args)...) {
    // Include any additional initialization here specific to Motor_GenericHBridge
  }

  /**
   * @brief Default destructor.
   */
  ~Motor_GenericHBridge() = default;

  // Note: any public methods from Base such as begin, update, setSpeed, stop,
  // brake, etc... are automatically available. Overwrite any methods here
  // if specific behavior is needed for this motor type.

 private:
  // Add any members specific to Motor_GenericHBridge here
};

#endif  // MOTOR_GENERICHBRIDGE