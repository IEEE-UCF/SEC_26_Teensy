/**
 * @file MotorTemplate.h
 * @brief Template for generic motor control with configurable pins, constants, and encoder
 * handling.
 * @authors Aldem Pido
 * @section edit_history Edit History
 * - 5/9/25 Created Class AP
 * - 5/9/25 Added type traits for method existence checks.
 * - 5/9/25 Aligned encoder method calls and traits to standard: beginEnc, getEnc, updateEnc,
 * resetEnc.
 * - 5/9/25 Changed private helper method WriteSpeed to writeSpeed for camelCase consistency.
 * - 5/9/25 Added boolean field for PWM direction to constants structs, updated WriteSpeed,
 * and ensured public methods follow camelCase.
 */
#ifndef MOTORTEMPLATE_H
#define MOTORTEMPLATE_H

// Assume Arduino.h or equivalent provides necessary functions (pinMode, analogWrite, etc.)
#include <Arduino.h>

// Standard C++ headers
#include <cmath>        // For std::abs
#include <type_traits>  // For std::is_same_v, std::enable_if_t, std::aligned_storage, std::void_t, std::true_type, std::false_type
#include <utility>      // For std::forward, std::declval

#include "../encoders/QuadEncoderObject.h"

// --- Forward Declarations ---
// Forward declare the main Motor class template
template <typename MotorType, typename EncoderType, typename ConstantsType, typename PinConfigType>
class Motor;

// --- Motor Type Tags ---
#pragma region Motor Type Tags
/**
 * @defgroup motor_tags Motor Type Tags
 * @brief Empty structs used as tags to define motor categories.
 */

/**
 * @ingroup motor_tags
 * @brief Tag struct representing a rotational motor type.
 *
 * Used as a template parameter for the Motor class to indicate
 * a rotational motor.
 */
struct RotationalMotor {};

/**
 * @ingroup motor_tags
 * @brief Tag struct representing a linear motor type.
 *
 * Used as a template parameter for the Motor class to indicate
 * a linear motor.
 */
struct LinearMotor {};
#pragma endregion  // Motor Type Tags

// --- Motor Encoder Types ---
#pragma region Encoder Types
/**
 * @defgroup encoder_types Encoder Types
 * @brief Structs and classes used as template parameters to define encoder types.
 *
 * Encoder types used with the Motor template should ideally implement
 * some or all of the standard methods: `beginEnc()`, `getEnc()`,
 * `updateEnc()`, and `resetEnc()`. Their presence is checked at
 * compile time using helper traits.
 */

/**
 * @ingroup encoder_types
 * @brief Tag struct representing no encoder type.
 *
 * Used as a template parameter for the Motor class when the motor has no encoder.
 * Encoder-related methods in the Motor template will be excluded via `if constexpr`.
 */
struct NoEncoder {};

/**
 * @ingroup encoder_types
 * @brief Forward declaration for a class that handles quadrature encoder signals.
 *
 * Used as a template parameter for the Motor class when using a quadrature encoder.
 * This class should implement the standard encoder methods: `beginEnc()`,
 * `getEnc()`, and `updateEnc()`.
 * Full documentation and definition are in QuadEncoderHandler.h.
 */
class QuadEncoderHandler;

/**
 * @ingroup encoder_types
 * @brief Struct representing an analog encoder connected to a single analog pin.
 *
 * Implements the standard `getEnc()` and `resetEnc()` methods for compatibility.
 */
class AnalogEncoder {
 private:
  uint8_t analogPin;  ///< The analog pin connected to the encoder signal.

 public:
  /**
   * @brief Constructs an AnalogEncoder.
   * @param analogPin The GPIO pin number for the analog encoder signal.
   */
  constexpr AnalogEncoder(uint8_t analogPin) : analogPin(analogPin) {}

  /**
   * @brief Gets a mapped position reading from the analog pin.
   *
   * Implements the standard `getEnc()` method. Reads the analog value from the
   * configured pin, maps it from the 0-1023 range to the 0-1000 range,
   * and returns the result.
   * @return The mapped analog position (0-1000).
   */
  long getEnc() const { return map(analogRead(analogPin), 0, 1023, 0, 1000); }

  /**
   * @brief Resets the analog encoder's conceptual position.
   *
   * Implements the standard `resetEnc()` method. This is typically a no-op
   * for simple analog encoders, as position is read directly from the pin,
   * but provided for compatibility.
   */
  void resetEnc() {}
};
#pragma endregion  // Encoder Types

// --- Motor Constant Structures ---
#pragma region Motor Constants
/**
 * @defgroup motor_constants Motor Constants
 * @brief Structs holding physical constants for various motor types.
 */

/**
 * @ingroup motor_constants
 * @brief Default struct for motors with no specific constants.
 *
 * Provides default values for common constants like max speed input and max PWM output,
 * and assumes high PWM corresponds to high speed.
 */
struct NoMotorConstants {
  float maxSpeedInput = 255.0f;    ///< The maximum absolute speed value expected by `setSpeed()`.
  int maxPWMOut = 255;             ///< The maximum absolute PWM output value (e.g., 255 for 8-bit).
  bool high_pwm_means_high_speed;  ///< True if higher PWM values mean higher speed, false if
                                   ///< inverse.

  /**
   * @brief Constructs NoMotorConstants.
   * @param maxInput Maximum input speed value.
   * @param maxPWM Maximum PWM output value.
   * @param highPWM High PWM value means high speed. Defaults to true.
   */
  constexpr NoMotorConstants(float maxInput = 255.0f, int maxPWM = 255, bool highPWM = true)
      : maxSpeedInput(maxInput), maxPWMOut(maxPWM), high_pwm_means_high_speed(highPWM) {}
};

/**
 * @ingroup motor_constants
 * @brief Struct for holding rotational motor constants.
 */
struct RotationalMotorConstants {
  float noLoadRPM;                 ///< No load RPM before gearing.
  float gearRatio;                 ///< The gear ratio of the motor/gearbox assembly.
  float maxSpeedInput;             ///< The maximum absolute speed value expected by `setSpeed()`.
  int maxPWMOut;                   ///< The maximum absolute PWM output value (e.g., 255 for 8-bit).
  bool high_pwm_means_high_speed;  ///< True if higher PWM values mean higher speed, false if
                                   ///< inverse.

  /**
   * @brief Constructs RotationalMotorConstants.
   * @param rpm No load RPM.
   * @param ratio Gear ratio.
   * @param maxInput Maximum input speed value. Defaults to 255.0f.
   * @param maxPWM Maximum PWM output value. Defaults to 255.
   * @param highPWM High PWM value means high speed. Defaults to true.
   */
  constexpr RotationalMotorConstants(float rpm, float ratio, float maxInput = 255.0f,
                                     int maxPWM = 255, bool highPWM = true)
      : noLoadRPM(rpm),
        gearRatio(ratio),
        maxSpeedInput(maxInput),
        maxPWMOut(maxPWM),
        high_pwm_means_high_speed(highPWM) {}
};

/**
 * @ingroup motor_constants
 * @brief Struct for holding linear motor constants.
 */
struct LinearMotorConstants {
  float strokeLength_mm;           ///< Total stroke length in mm.
  float maxSpeed_mm_s;             ///< Maximum speed in mm/s.
  float maxSpeedInput;             ///< The maximum absolute speed value expected by `setSpeed()`.
  int maxPWMOut;                   ///< The maximum absolute PWM output value (e.e., 255 for 8-bit).
  bool high_pwm_means_high_speed;  ///< True if higher PWM values mean higher speed, false if
                                   ///< inverse.

  /**
   * @brief Constructs LinearMotorConstants.
   * @param stroke Stroke length in mm.
   * @param speed_mm_s Max speed in mm/s.
   * @param maxInput Maximum input speed value. Defaults to 255.0f.
   * @param maxPWM Maximum PWM output value. Defaults to 255.
   * @param highPWM High PWM value means high speed. Defaults to true.
   */
  constexpr LinearMotorConstants(float stroke, float speed_mm_s, float maxInput = 255.0f,
                                 int maxPWM = 255, bool highPWM = true)
      : strokeLength_mm(stroke),
        maxSpeed_mm_s(speed_mm_s),
        maxSpeedInput(maxInput),
        maxPWMOut(maxPWM),
        high_pwm_means_high_speed(highPWM) {}
};
#pragma endregion  // Motor Constants

// --- Pin Configuration Structures ---
#pragma region Motor Pin Configurations
/**
 * @defgroup pin_config Motor Pin Configurations
 * @brief Structs defining motor driver pin layouts.
 */

/**
 * @ingroup pin_config
 * @brief Struct for a speed-only pin motor configuration.
 *
 * Typically for unidirectional motors controlled by a single PWM pin.
 */
struct OneOnlySpeed {
  uint8_t kSpeed;  ///< Speed (PWM) pin.
  /**
   * @brief Constructs OneOnlySpeed config.
   * @param speedPin Speed pin number.
   */
  constexpr OneOnlySpeed(uint8_t speedPin) : kSpeed(speedPin) {}
};

/**
 * @ingroup pin_config
 * @brief Struct for a speed and direction motor configuration.
 *
 * Typically for bidirectional motors controlled by a PWM pin and a digital direction pin.
 */
struct TwoSpeedDir {
  uint8_t kSpeed;  ///< Speed (PWM) pin.
  uint8_t kDir;    ///< Direction (digital) pin.
  /**
   * @brief Constructs TwoSpeedDir config.
   * @param speedPin Speed pin number.
   * @param dirPin Direction pin number.
   */
  constexpr TwoSpeedDir(uint8_t speedPin, uint8_t dirPin) : kSpeed(speedPin), kDir(dirPin) {}
};

/**
 * @ingroup pin_config
 * @brief Struct for a speed, direction, and enable motor configuration.
 *
 * Commonly found in motor driver ICs (e.e., L298N in some modes).
 */
struct ThreeSpeedDirEn {
  uint8_t kSpeed;  ///< Speed (PWM) pin.
  uint8_t kDir;    ///< Direction (digital) pin.
  uint8_t kEn;     ///< Enable (digital) pin.
  /**
   * @brief Constructs ThreeSpeedDirEn config.
   * @param speedPin Speed pin number.
   * @param dirPin Direction pin number.
   * @param enPin Enable pin number.
   */
  constexpr ThreeSpeedDirEn(uint8_t speedPin, uint8_t dirPin, uint8_t enPin)
      : kSpeed(speedPin), kDir(dirPin), kEn(enPin) {}
};

/**
 * @ingroup pin_config
 * @brief Struct for a speed and dual direction motor configuration (H-bridge style).
 *
 * Uses a PWM pin for speed and two digital pins for direction control (e.g., IN1/IN2).
 */
struct ThreeSpeedDirDir {
  uint8_t kSpeed;  ///< Speed (PWM) pin.
  uint8_t kDirA;   ///< Direction A (digital) pin (e.g., IN1).
  uint8_t kDirB;   ///< Direction B (digital) pin (e.g., IN2).
  /**
   * @brief Constructs ThreeSpeedDirDir config.
   * @param speedPin Speed pin number.
   * @param dirAPin Direction A pin number.
   * @param dirBPin Direction B pin number.
   */
  constexpr ThreeSpeedDirDir(uint8_t speedPin, uint8_t dirAPin, uint8_t dirBPin)
      : kSpeed(speedPin), kDirA(dirAPin), kDirB(dirBPin) {}
};

/**
 * @ingroup pin_config
 * @brief Struct for a speed and dual direction with enable motor configuration (H-bridge style).
 *
 * Uses a PWM pin for speed, two digital pins for direction, and a digital enable pin.
 */
struct FourSpeedDirDirEn {  // H-bridge style with enable
  uint8_t kSpeed;           ///< Speed (PWM) pin.
  uint8_t kDirA;            ///< Direction A (digital) pin (e.g., IN1).
  uint8_t kDirB;            ///< Direction B (digital) pin (e.g., IN2).
  uint8_t kEn;              ///< Enable (digital) pin.
  /**
   * @brief Constructs FourSpeedDirDirEn config.
   * @param speedPin Speed pin number.
   * @param dirAPin Direction A pin number.
   * @param dirBPin Direction B pin number.
   * @param enPin Enable pin number.
   */
  constexpr FourSpeedDirDirEn(uint8_t speedPin, uint8_t dirAPin, uint8_t dirBPin, uint8_t enPin)
      : kSpeed(speedPin), kDirA(dirAPin), kDirB(dirBPBPin), kEn(enPin) {}
};
#pragma endregion  // Pin Configurations

// --- Helper Traits for Method Detection (C++17 Detection Idiom) ---
// These traits check if a type has a specific member function callable with no arguments.
// This allows using if constexpr to conditionally call methods on the EncoderType.
#pragma region Helper Traits
/**
 * @defgroup motor_helper_traits Motor Helper Traits
 */

/**
 * @ingroup motor_helper_traits
 * @brief Helper trait to check if a type has a `beginEnc()` member function.
 */
template <typename, typename = std::void_t<>>
struct has_method_beginEnc_impl : std::false_type {};
template <typename T>
struct has_method_beginEnc_impl<T, std::void_t<decltype(std::declval<T>().beginEnc())>>
    : std::true_type {};
template <typename T>
constexpr bool has_method_beginEnc_v = has_method_beginEnc_impl<T>::value;

/**
 * @ingroup motor_helper_traits
 * @brief Helper trait to check if a type has an `updateEnc()` member function.
 */
template <typename, typename = std::void_t<>>
struct has_method_updateEnc_impl : std::false_type {};
template <typename T>
struct has_method_updateEnc_impl<T, std::void_t<decltype(std::declval<T>().updateEnc())>>
    : std::true_type {};
template <typename T>
constexpr bool has_method_updateEnc_v = has_method_updateEnc_impl<T>::value;

/**
 * @ingroup motor_helper_traits
 * @brief Helper trait to check if a type has a `getEnc()` member function.
 */
template <typename, typename = std::void_t<>>
struct has_method_getEnc_impl : std::false_type {};
template <typename T>
struct has_method_getEnc_impl<T, std::void_t<decltype(std::declval<T>().getEnc())>>
    : std::true_type {};
template <typename T>
constexpr bool has_method_getEnc_v = has_method_getEnc_impl<T>::value;

/**
 * @ingroup motor_helper_traits
 * @brief Helper trait to check if a type has a `resetEnc()` member function.
 */
template <typename, typename = std::void_t<>>
struct has_method_resetEnc_impl : std::false_type {};
template <typename T>
struct has_method_resetEnc_impl<T, std::void_t<decltype(std::declval<T>().resetEnc())>>
    : std::true_type {};
template <typename T>
constexpr bool has_method_resetEnc_v = has_method_resetEnc_impl<T>::value;
#pragma endregion  // Helper Traits

// --- Main Template Definition and Implementation ---
#pragma region Definition
/**
 * @defgroup motor_template Generic Motor Template
 * @brief A generic template for defining various motor types and their control.
 *
 * This group contains the core Motor class template, helper traits used for
 * conditional compilation, and base types for defining motor constants,
 * encoders, and pin configurations.
 */

/**
 * @ingroup motor_template
 * @brief Generic template class for motor control.
 *
 * This template provides a flexible framework for controlling different
 * types of motors by abstracting common functionalities and allowing
 * customization through template parameters for motor type, encoder,
 * constants, and pin configuration.
 *
 * It uses `if constexpr` and method detection traits to conditionally compile
 * code based on the provided `EncoderType` and `PinConfigType`, ensuring
 * type safety and efficiency by only including the necessary logic for the
 * specific motor/encoder combination.
 *
 * @tparam MotorType A tag type indicating the motor's physical nature (e.g., RotationalMotor,
 * LinearMotor).
 * @tparam EncoderType The type of encoder used with the motor (e.g., QuadEncoderHandler,
 * AnalogEncoder, NoEncoder). Use `NoEncoder` if no encoder is present. Encoder types
 * should implement methods like `beginEnc()`, `getEnc()`, `updateEnc()`, and `resetEnc()`
 * where applicable.
 * @tparam ConstantsType A struct or class holding physical constants for the motor (e.g., max
 * speed input, max PWM output). Must have public `maxSpeedInput`, `maxPWMOut`, and
 * `high_pwm_means_high_speed` members.
 * @tparam PinConfigType A struct or class holding pin assignments and defining the motor driver's
 * interface (e.g., OneOnlySpeed, TwoSpeedDir). Determines the `pinMode` and `writeSpeed` logic.
 * Must have public members corresponding to the chosen configuration type (e.e., `kSpeed`, `kDir`).
 */
template <typename MotorType, typename EncoderType, typename ConstantsType, typename PinConfigType>
class Motor {
 public:
  // --- Type Aliases ---
  using MotorType_t = MotorType;          ///< Alias for the MotorType template parameter.
  using EncoderType_t = EncoderType;      ///< Alias for the EncoderType template parameter.
  using ConstantsType_t = ConstantsType;  ///< Alias for the ConstantsType template parameter.
  using PinConfigType_t = PinConfigType;  ///< Alias for the PinConfigType template parameter.

  // --- Disable Copy and Move ---
  // Explicitly delete copy/move operations because motor objects are tied to hardware resources.
  Motor(const Motor &) = delete;             ///< Deleted copy constructor.
  Motor &operator=(const Motor &) = delete;  ///< Deleted copy assignment operator.
  Motor(Motor &&) = delete;                  ///< Deleted move constructor.
  Motor &operator=(Motor &&) = delete;       ///< Deleted move assignment operator.

  // --- Constructor ---
  /**
   * @brief Constructs a Motor instance.
   *
   * Initializes the motor with its physical constants and pin configuration.
   * Based on `PinConfigType`, it configures the necessary GPIO pins using `pinMode`
   * and sets initial states where applicable (e.g., enable pins low).
   * If `EncoderType` is not `NoEncoder`, it constructs the encoder object
   * in allocated storage using placement new and forwarding the provided
   * variadic arguments.
   *
   * @tparam T_Constants Type of the constants argument (deduced by perfect forwarding).
   * @tparam T_PinConfig Type of the pin configuration argument (deduced by perfect forwarding).
   * @tparam EncoderArgs Types of the variadic arguments for the encoder constructor (deduced by
   * perfect forwarding).
   *
   * @param constants Physical constants for the motor. Passed by forwarding reference
   * (`std::forward`) to allow passing lvalues or rvalues efficiently. Must be convertible
   * to `ConstantsType`.
   * @param pin_config Pin assignments and configuration for the motor hardware. Passed by
   * forwarding reference (`std::forward`). Must be convertible to `PinConfigType`.
   * @param encoder_args Variadic arguments forwarded to the constructor of `EncoderType` if
   * `EncoderType` is not `NoEncoder`. The constructor `EncoderType::EncoderType(EncoderArgs...)`
   * must be valid.
   */
  template <typename T_Constants, typename T_PinConfig, typename... EncoderArgs>
  Motor(T_Constants &&constants, T_PinConfig &&pin_config, EncoderArgs &&...encoder_args)
      : constants_(std::forward<T_Constants>(constants)),
        pin_config_(std::forward<T_PinConfig>(pin_config)),
        encoder_storage_() {
    // --- Constructor Body ---
    // Initialize pins based on PinConfigType using if constexpr
    if constexpr (std::is_same_v<PinConfigType, OneOnlySpeed>) {
      pinMode(pin_config_.kSpeed, OUTPUT);
    } else if constexpr (std::is_same_v<PinConfigType, TwoSpeedDir>) {
      pinMode(pin_config_.kSpeed, OUTPUT);
      pinMode(pin_config_.kDir, OUTPUT);
    } else if constexpr (std::is_same_v<PinConfigType, ThreeSpeedDirEn>) {
      pinMode(pin_config_.kSpeed, OUTPUT);
      pinMode(pin_config_.kDir, OUTPUT);
      pinMode(pin_config_.kEn, OUTPUT);
      digitalWrite(pin_config_.kEn, LOW);  // Initialize enable pin low
    } else if constexpr (std::is_same_v<PinConfigType, ThreeSpeedDirDir>) {
      pinMode(pin_config_.kSpeed, OUTPUT);
      pinMode(pin_config_.kDirA, OUTPUT);
      pinMode(pin_config_.kDirB, OUTPUT);
    } else if constexpr (std::is_same_v<PinConfigType, FourSpeedDirDirEn>) {
      pinMode(pin_config_.kSpeed, OUTPUT);
      pinMode(pin_config_.kDirA, OUTPUT);
      pinMode(pin_config_.kDirB, OUTPUT);
      pinMode(pin_config_.kEn, OUTPUT);
      digitalWrite(pin_config_.kEn, LOW);  // Initialize enable pin low
    } else {
      static_assert(false, "Unsupported PinConfigType for pin initialization in Motor constructor");
    }

    // --- Construct Encoder Member ---
    // Will only construct if the encoder is not NoEncoder. Uses placement new.
    if constexpr (!std::is_same_v<EncoderType, NoEncoder>) {
      new (&encoder_storage_)
          EncoderType(std::forward<EncoderArgs>(encoder_args)...);  // <-- Target changed
    }
  }

  // --- Destructor ---

  /**
   * @brief Destroys the Motor instance.
   *
   * If an encoder was constructed (i.e., `EncoderType` is not `NoEncoder`),
   * this explicitly calls the encoder's destructor using its pre-allocated storage.
   */
  ~Motor() {
    // If EncoderType is not NoEncoder, explicitly call the destructor of the encoder object
    if constexpr (!std::is_same_v<EncoderType, NoEncoder>) {
      reinterpret_cast<EncoderType *>(&encoder_storage_)
          ->~EncoderType();  // <-- Method of calling destructor changed
    }
    // Note: Member variables constants_ and pin_config_ have their destructors
    // called automatically after the explicit encoder destructor call.
  }

  // --- Initialization Method ---
  /**
   * @brief Performs post-construction initialization.
   *
   * Initializes the encoder if it's not `NoEncoder` and calls its `beginEnc()`
   * method if the `EncoderType` has one (e.g., for QuadEncoderHandler). Sets
   * the initial motor state to stopped.
   */
  void begin() {  // Changed from Begin
    // Initialize encoders if EncoderType is not NoEncoder
    if constexpr (!std::is_same_v<EncoderType, NoEncoder>) {
      // Check if the encoder type has a 'beginEnc' method and call it if it exists
      if constexpr (has_method_beginEnc_v<EncoderType>) {
        getOwnedEncoder().beginEnc();  // Call the beginEnc method on the encoder object
      }
      // Add other method existence checks and calls for specific encoder types if they have a
      // 'beginEnc' method variant
    }

    // Initialize motor state
    if constexpr (std::is_same_v<PinConfigType, ThreeSpeedDirEn> ||
                  std::is_same_v<PinConfigType, FourSpeedDirDirEn>) {
      digitalWrite(pin_config_.kEn, LOW);  // Ensure motor enable pin is low initially
    }
    stop();  // Changed from Stop
  }

  // --- Update Method ---
  /**
   * @brief Performs periodic updates for the motor and encoder.
   *
   * If `EncoderType` is not `NoEncoder` and has an `updateEnc()` method,
   * this method calls `encoder_.updateEnc()`. This function should be called
   * regularly in the main loop to process encoder signals or perform
   * other time-sensitive updates for the encoder or motor.
   */
  void update() {  // Changed from Update
    if constexpr (!std::is_same_v<EncoderType, NoEncoder>) {
      // Check if the encoder type has an 'updateEnc' method and call it if it exists
      if constexpr (has_method_updateEnc_v<EncoderType>) {
        getOwnedEncoder().updateEnc();  // Call the updateEnc method on the encoder object
      }
      // Add calls for other specific encoder types if needed using similar method existence
      // checks
    }
    // Add motor-specific update logic here if any (e.g., PID control loops that need periodic
    // updates)
  }

  // --- Motor Control Methods ---
  /**
   * @brief Sets the desired motor speed using a scaled input value.
   *
   * The input `speed_value` is expected to be within the range
   * [-constants_.maxSpeedInput, constants_.maxSpeedInput].
   * The value is clamped to this range and then scaled to a raw PWM command
   * which is applied to the motor pins using the specific pin configuration.
   *
   * @param speed_value The desired speed input value, typically representing units relevant to
   * the motor (e.g., RPM, units/sec).
   */
  void setSpeed(float speed_value) {  // Changed from SetSpeed
    // Ensure speed is within the valid input range defined by constants
    speed_value = constrain(speed_value, -constants_.maxSpeedInput, constants_.maxSpeedInput);

    // Calculate the raw PWM command [-maxPWMOut, maxPWMOut]
    int motor_command = calculateMotorCommand(speed_value);

    // Apply the command using the specific pin configuration
    writeSpeed(motor_command);
  }

  /**
   * @brief Sets the desired motor speed using a percentage.
   *
   * The input `speed_percentage` is expected to be within the range [-100.0, 100.0].
   * This percentage is converted to the internal speed value range
   * [-constants_.maxSpeedInput, constants_.maxSpeedInput] and then
   * the `setSpeed` method is called.
   *
   * @param speed_percentage The desired speed as a percentage of maximum input speed ([-100.0,
   * 100.0]).
   */
  void setSpeedPercentage(float speed_percentage) {  // Changed from SetSpeedPercentage
    // Ensure percentage is within the valid range [-100.0, 100.0]
    speed_percentage = constrain(speed_percentage, -100.0, 100.0);

    // Convert percentage to the internal speed value range
    float speed_value = (speed_percentage / 100.0) * constants_.maxSpeedInput;

    // Call the setSpeed method with the calculated speed value
    setSpeed(speed_value);
  }

  /**
   * @brief Stops the motor.
   *
   * Applies a raw motor command of 0 using the `writeSpeed` method,
   * effectively stopping the motor's movement.
   */
  void stop() {  // Changed from Stop
    writeSpeed(0);
  }

  /**
   * @brief Engages braking for the motor.
   *
   * The specific braking implementation depends on the `PinConfigType`.
   * This typically involves setting direction pins to a specific state and
   * setting the speed/PWM to 0 using the `writeSpeed` helper. For configurations
   * that don't support braking directly, it defaults to calling `stop()`.
   */
  void brake() {  // Changed from Brake
    // Use if constexpr to select braking logic based on PinConfigType
    if constexpr (std::is_same_v<PinConfigType, TwoSpeedDir>) {
      digitalWrite(pin_config_.kDir,
                   HIGH);  // Example braking state for 2-pin (coast/brake depends on driver)
      analogWrite(pin_config_.kSpeed, 0);  // PWM to 0
    } else if constexpr (std::is_same_v<PinConfigType, ThreeSpeedDirEn>) {
      digitalWrite(pin_config_.kEn, HIGH);   // Enable motor (necessary for some brake modes)
      digitalWrite(pin_config_.kDir, HIGH);  // Example braking state for 3-pin (Enable + Dir)
      analogWrite(pin_config_.kSpeed, 0);    // PWM to 0
    } else if constexpr (std::is_same_v<PinConfigType, ThreeSpeedDirDir>) {
      digitalWrite(pin_config_.kDirA,
                   HIGH);  // Example braking state for 3-pin (DirA + DirB) - typically brake
      digitalWrite(pin_config_.kDirB, HIGH);
      analogWrite(pin_config_.kSpeed, 0);  // PWM to 0
    } else if constexpr (std::is_same_v<PinConfigType, FourSpeedDirDirEn>) {
      digitalWrite(pin_config_.kEn, HIGH);    // Enable motor
      digitalWrite(pin_config_.kDirA, HIGH);  // Example braking state for 4-pin - typically brake
      digitalWrite(pin_config_.kDirB, HIGH);
      analogWrite(pin_config_.kSpeed, 0);  // PWM to 0
    } else {
      // For configurations that don't have dedicated brake pins/logic, default to stopping
      stop();
    }
  }

  // --- Encoder-related methods (conditionally available via SFINAE) ---

  /**
   * @brief Gets the current position from the encoder.
   *
   * This method is only available if `EncoderType` is not `NoEncoder` and has a `getEnc()` method.
   * It calls the `getEnc()` method on the encoder object.
   *
   * @tparam T Deduced type, used for SFINAE. Do not specify explicitly.
   * @return The current position as reported by the encoder (`long`). The interpretation
   * depends on the specific encoder type.
   * @sa resetEncoder()
   * @note This method uses `std::enable_if_t` to be conditionally compiled
   * only when `EncoderType` is not `NoEncoder` and has a `getEnc()` method.
   */
  template <typename T = EncoderType>
  auto getEncoderPosition()
      -> std::enable_if_t<!std::is_same_v<T, NoEncoder> &&
                              has_method_getEnc_v<T>,  // Check both NoEncoder and getEnc()
                                                       // existence
                          long> const {
    // Use if constexpr and method traits to call the correct method if it exists
    // The SFINAE already guarantees getEnc exists if we reach here, but double-checking is fine
    if constexpr (has_method_getEnc_v<EncoderType>) {
      getOwnedEncoder().getEnc();  // Call getEnc() on the encoder object
    } else {
      // This static_assert should theoretically not be reachable if SFINAE works correctly
      // and the outer if constexpr guards are correct, but left as a fallback.
      static_assert(false,
                    "EncoderType is not NoEncoder but has no getEnc() method (should have been "
                    "caught by SFINAE).");
      return 0;  // Should not be reached
    }
  }

  /**
   * @brief Resets the encoder's position count.
   *
   * This method is only available if `EncoderType` is not `NoEncoder` and has a `resetEnc()`
   * method. It calls the `resetEnc()` method on the encoder object.
   *
   * @tparam T Deduced type, used for SFINAE. Do not specify explicitly.
   * @note This method uses `std::enable_if_t` to be conditionally compiled
   * only when `EncoderType` is not `NoEncoder` and has a `resetEnc()` method.
   */
  template <typename T = EncoderType>
  auto resetEncoder() -> std::enable_if_t<!std::is_same_v<T, NoEncoder> &&
                                          has_method_resetEnc_v<T>>  // Check both NoEncoder and
                                                                     // resetEnc() existence
  {
    // Check if the encoder type has a 'resetEnc' method and call it if it exists
    if constexpr (has_method_resetEnc_v<EncoderType>) {
      getOwnedEncoder().resetEnc();  // Call resetEnc() on the encoder object
    } else {
      // This static_assert should theoretically not be reachable if SFINAE works correctly
      // and the outer if constexpr guards are correct, but left as a fallback.
      static_assert(false,
                    "EncoderType is not NoEncoder but has no resetEnc() method (should have been "
                    "caught by SFINAE).");
    }
  }

  // --- Accessor Methods ---

  /**
   * @brief Gets a const reference to the motor's constants.
   * @return A const reference to the constants structure (`ConstantsType`).
   */
  const ConstantsType &getConstants() const { return constants_; }

  /**
   * @brief Gets a const reference to the motor's pin configuration.
   * @return A const reference to the pin configuration structure (`PinConfigType`).
   */
  const PinConfigType &getPinConfig() const { return pin_config_; }

 private:
  // --- Member Variables ---
  ConstantsType constants_;   ///< Stores the motor's physical constants.
  PinConfigType pin_config_;  ///< Stores the motor's hardware pin assignments and configuration.
  /**
   * @brief Raw storage for the encoder object.
   *
   * Used with placement new in the constructor to create the encoder object
   * only if `EncoderType` is not `NoEncoder`. Provides memory with correct
   * size and alignment for `EncoderType`. Initialized to zeros by default initialization.
   */
  typename std::aligned_storage<sizeof(EncoderType), alignof(EncoderType)>::type encoder_storage_;

  // --- Helper Methods (Private) ---

  /**
   * @brief Safely accesses the owned encoder object stored in `encoder_storage_`.
   *
   */
  EncoderType &getOwnedEncoder() { return *reinterpret_cast<EncoderType *>(&encoder_storage_); }

  /**
   * @brief Safely accesses the owned encoder object stored in `encoder_storage_` (const version).
   *
   */
  const EncoderType &getOwnedEncoder() const {
    return *reinterpret_cast<const EncoderType *>(&encoder_storage_);
  }

  /**
   * @brief Calculates the raw motor command from a speed input value.
   *
   * Scales the input `speed_input` from the range [-constants_.maxSpeedInput,
   * constants_.maxSpeedInput] to the raw output range [-maxPWMOut, maxPWMOut] as defined by
   * `constants_`. The result is clamped to the integer PWM output range. Assumes `ConstantsType`
   * has public `maxSpeedInput` and `maxPWMOut` members. Includes compile-time assertions to
   * validate constant values if `ConstantsType` is one of the known types
   * (`RotationalMotorConstants`, `LinearMotorConstants`, `NoMotorConstants`).
   *
   * @param speed_input The input speed value within the range [-constants_.maxSpeedInput,
   * constants_.maxSpeedInput].
   * @return An integer motor command value, clamped to the range [-constants_.maxPWMOut,
   * constants_.maxPWMOut].
   */
  int calculateMotorCommand(float speed_input) const {
    // Ensure maxSpeedInput and maxPWMOut are valid for scaling using static_assert within if
    // constexpr This check is compile-time and only applies if ConstantsType matches one of the
    // known types
    if constexpr (std::is_same_v<ConstantsType, RotationalMotorConstants> ||
                  std::is_same_v<ConstantsType, LinearMotorConstants> ||
                  std::is_same_v<ConstantsType, NoMotorConstants>) {  // Also check NoMotorConstants
      static_assert(ConstantsType::maxSpeedInput > 0,
                    "ConstantsType::maxSpeedInput must be greater than 0");
      static_assert(ConstantsType::maxPWMOut > 0,
                    "ConstantsType::maxPWMOut must be greater than 0");  // Assuming typical PWM max
    }
    // Note: If ConstantsType doesn't match these or define the members, the compiler
    // will typically give an error when trying to access constants_.maxSpeedInput/maxPWMOut
    // unless there's a more generic fallback or requirement specified elsewhere.

    // Scale the input speed to the PWM output range
    // Handles negative speeds and ensures output is within [-maxPWMOut, maxPWMOut]
    float scaled_command =
        (speed_input / constants_.maxSpeedInput) * static_cast<float>(constants_.maxPWMOut);

    // Return as int, clamping to the valid integer range [-maxPWMOut, maxPWMOut]
    return static_cast<int>(constrain(scaled_command, -static_cast<float>(constants_.maxPWMOut),
                                      static_cast<float>(constants_.maxPWMOut)));
  }

  /**
   * @brief Applies the raw motor command to the hardware pins.
   *
   * This helper method translates the integer `pwm_command` [-maxPWMOut, maxPWMOut]
   * into specific digital and analog writes to the motor pins based on the
   * `PinConfigType` using `if constexpr`.
   * The direction of PWM mapping (high value = high speed or inverse) is
   * determined by the `high_pwm_means_high_speed` constant.
   *
   * @param pwm_command The raw integer motor command to apply.
   */
  void writeSpeed(int pwm_command) {  // Changed from WriteSpeed
    // Use if constexpr to select pin control logic based on PinConfigType
    if constexpr (std::is_same_v<PinConfigType, OneOnlySpeed>) {
      // Control for one-pin config (e.g., simple PWM, likely unidirectional)
      // Assumes pwm_command sign might indicate direction if driver supports it
      // implicitly, or only positive commands are expected. Using abs() for
      // simple PWM magnitude.
      uint8_t pwm_value = std::abs(pwm_command);
      if (!constants_.high_pwm_means_high_speed) {  // Apply inverse PWM if needed
        pwm_value = constants_.maxPWMOut - pwm_value;
      }
      analogWrite(pin_config_.kSpeed, pwm_value);
    } else if constexpr (std::is_same_v<PinConfigType, TwoSpeedDir>) {
      // Control for two-pin config (e.g., PWM + Digital Direction)
      uint8_t pwm_value = std::abs(pwm_command);
      if (!constants_.high_pwm_means_high_speed) {  // Apply inverse PWM if needed
        pwm_value = constants_.maxPWMOut - pwm_value;
      }
      if (pwm_command >= 0) {  // Positive command for forward
        digitalWrite(pin_config_.kDir,
                     HIGH);  // Adjust HIGH/LOW based on wiring for forward
        analogWrite(pin_config_.kSpeed, pwm_value);
      } else {                                // Negative command for reverse
        digitalWrite(pin_config_.kDir, LOW);  // Adjust HIGH/LOW based on wiring for reverse
        analogWrite(pin_config_.kSpeed, pwm_value);
      }
    } else if constexpr (std::is_same_v<PinConfigType, ThreeSpeedDirEn>) {
      // Control for three-pin config (e.g., PWM + Digital Direction + Digital Enable)
      uint8_t pwm_value = std::abs(pwm_command);
      if (!constants_.high_pwm_means_high_speed) {  // Apply inverse PWM if needed
        pwm_value = constants_.maxPWMOut - pwm_value;
      }
      if (pwm_value == 0) {
        digitalWrite(pin_config_.kEn, LOW);  // Disable motor when command is 0
        analogWrite(pin_config_.kSpeed, 0);  // Ensure speed is 0
      } else {
        digitalWrite(pin_config_.kEn, HIGH);     // Enable motor
        if (pwm_command > 0) {                   // Positive command for forward
          digitalWrite(pin_config_.kDir, HIGH);  // Adjust HIGH/LOW for forward
        } else {                                 // Negative command for reverse
          digitalWrite(pin_config_.kDir, LOW);   // Adjust HIGH/LOW for reverse
        }
        analogWrite(pin_config_.kSpeed, pwm_value);
      }
    } else if constexpr (std::is_same_v<PinConfigType, ThreeSpeedDirDir>) {
      // Control for H-bridge style (PWM + two direction pins)
      uint8_t pwm_value = std::abs(pwm_command);
      if (!constants_.high_pwm_means_high_speed) {  // Apply inverse PWM if needed
        pwm_value = constants_.maxPWMOut - pwm_value;
      }
      if (pwm_command == 0) {
        digitalWrite(pin_config_.kDirA, LOW);
        digitalWrite(pin_config_.kDirB,
                     LOW);  // Often results in coasting or brake depending on driver
        analogWrite(pin_config_.kSpeed, 0);     // Ensure speed is 0
      } else if (pwm_command > 0) {             // Positive command for forward
        digitalWrite(pin_config_.kDirA, HIGH);  // Forward
        digitalWrite(pin_config_.kDirB, LOW);
        analogWrite(pin_config_.kSpeed, pwm_value);
      } else {                                 // Negative command for reverse
        digitalWrite(pin_config_.kDirA, LOW);  // Reverse
        digitalWrite(pin_config_.kDirB, HIGH);
        analogWrite(pin_config_.kSpeed, pwm_value);
      }
    } else if constexpr (std::is_same_v<PinConfigType, FourSpeedDirDirEn>) {
      // Control for H-bridge style with Enable
      uint8_t pwm_value = std::abs(pwm_command);
      if (!constants_.high_pwm_means_high_speed) {  // Apply inverse PWM if needed
        pwm_value = constants_.maxPWMOut - pwm_value;
      }
      if (pwm_value == 0) {
        digitalWrite(pin_config_.kEn, LOW);    // Disable
        digitalWrite(pin_config_.kDirA, LOW);  // Ensure dir pins are low
        digitalWrite(pin_config_.kDirB, LOW);
        analogWrite(pin_config_.kSpeed, 0);  // Ensure speed is 0
      } else {
        digitalWrite(pin_config_.kEn, HIGH);      // Enable
        if (pwm_command > 0) {                    // Positive command for forward
          digitalWrite(pin_config_.kDirA, HIGH);  // Forward
          digitalWrite(pin_config_.kDirB, LOW);
        } else {                                 // Negative command for reverse
          digitalWrite(pin_config_.kDirA, LOW);  // Reverse
          digitalWrite(pin_config_.kDirB, HIGH);
        }
        analogWrite(pin_config_.kSpeed, pwm_value);
      }
    } else {
      // This static_assert ensures that if a new PinConfigType is added,
      // its WriteSpeed logic needs to be handled here.
      static_assert(false, "Unsupported PinConfigType for pin control in writeSpeed");
    }
  }
};

#pragma endregion

#endif  // MOTORTEMPLATE_H