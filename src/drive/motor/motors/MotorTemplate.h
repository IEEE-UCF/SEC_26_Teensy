/*
MotorTemplate.h - Template for generic motor control with configurable
pins, constants, and Encoder handling

Description:
Declares a C++ template for creating highly flexible motor objects with compile-time
configuration of motor type (rotational/linear), physical constants, pin assignments,
and encoder type.

Begin and Write are represented by the Motor::Begin() method for initialization
and Motor::SetSpeed() method for setting motor speed.

Edit history:
5/8/25 - Begin tracking - Aldem Pido
5/9/25 - Completed template structure with pin configs, constants, encoder handling - Gemini
5/9/25 - Integrated external QuadEncoderHandler - Gemini
5/9/25 - Renamed methods to uppercase, changed speed input scale, added PWM output range - Gemini
*/
#ifndef MOTORTEMPLATE_H
#define MOTORTEMPLATE_H

#include <Arduino.h>
#include <type_traits> // For std::is_same_v, std::enable_if_t
#include <utility>     // For std::forward
#include <cmath>       // For std::abs

// Include the header for your external QuadEncoder handler
// This header must define the QuadEncoderHandler class and QuadEncoderSetup struct
#include "../encoders/QuadEncoderHandler.h"

// --- Forward Declarations ---
// Forward declare the main Motor class template
template <
    typename MotorType,
    typename EncoderType,
    typename ConstantsType,
    typename PinConfigType>
class Motor;

// --- Motor Type Tags ---
struct RotationalMotor
{
};
struct LinearMotor
{
};
// Add more as needed

// --- Encoder Types ---
struct NoEncoder
{
}; // Type tag for no encoder

// QuadratureEncoderHandler class definition is in "QuadEncoderHandler.h"

// AnalogEncoder Class (defined here; could be external)
class AnalogEncoder
{
private:
    uint8_t analogPin;

public:
    constexpr AnalogEncoder(uint8_t analogPin) : analogPin(analogPin) {}
    long GetPosition() const { return map(analogRead(analogPin), 0, 1023, 0, 1000); }
    void Reset() {}
};

// --- Motor Constant Structures ---
// Now includes maxSpeedInput and maxPWMOut
struct NoMotorConstants
{
}; // Default for motors with no specific constants

struct RotationalMotorConstants
{
    float noLoadRPM;
    float gearRatio;
    float maxSpeedInput; // e.g., 1.0 for -1.0 to 1.0 range, or 255.0 for -255 to 255 range
    int maxPWMOut;       // e.g., 255 for 8-bit PWM

    constexpr RotationalMotorConstants(float rpm, float ratio, float maxInput, int maxPWM)
        : noLoadRPM(rpm), gearRatio(ratio), maxSpeedInput(maxInput), maxPWMOut(maxPWM) {}
};

struct LinearMotorConstants
{
    float strokeLength_mm;
    float maxSpeed_mm_s;
    float maxSpeedInput; // e.g., 1.0 for -1.0 to 1.0 range, or maxSpeed_mm_s
    int maxPWMOut;       // e.g., 255 for 8-bit PWM

    constexpr LinearMotorConstants(float stroke, float speed_mm_s, float maxInput, int maxPWM)
        : strokeLength_mm(stroke), maxSpeed_mm_s(speed_mm_s), maxSpeedInput(maxInput), maxPWMOut(maxPWM) {}
};

// --- Pin Configuration Structures ---
struct OneOnlySpeed
{
    uint8_t kSpeed;
    constexpr OneOnlySpeed(uint8_t speedPin) : kSpeed(speedPin) {}
};
struct TwoSpeedDir
{
    uint8_t kSpeed;
    uint8_t kDir;
    constexpr TwoSpeedDir(uint8_t speedPin, uint8_t dirPin) : kSpeed(speedPin), kDir(dirPin) {}
};
struct ThreeSpeedDirEn
{
    uint8_t kSpeed;
    uint8_t kDir;
    uint8_t kEn;
    constexpr ThreeSpeedDirEn(uint8_t speedPin, uint8_t dirPin, uint8_t enPin) : kSpeed(speedPin), kDir(dirPin), kEn(enPin) {}
};
struct ThreeSpeedDirDir
{                   // H-bridge style control with separate forward/reverse pins + speed
    uint8_t kSpeed; // PWM pin
    uint8_t kDirA;  // e.g., Forward pin
    uint8_t kDirB;  // e.g., Reverse pin
    constexpr ThreeSpeedDirDir(uint8_t speedPin, uint8_t dirAPin, uint8_t dirBPin)
        : kSpeed(speedPin), kDirA(dirAPin), kDirB(dirBPin) {} // Corrected kDirBPB
};
struct FourSpeedDirDirEn
{ // H-bridge style with enable
    uint8_t kSpeed;
    uint8_t kDirA;
    uint8_t kDirB;
    uint8_t kEn;
    constexpr FourSpeedDirDirEn(uint8_t speedPin, uint8_t dirAPin, uint8_t dirBPin, uint8_t enPin)
        : kSpeed(speedPin), kDirA(dirAPin), kDirB(dirBPin), kEn(enPin) {}
};

// --- Main Template Definition and Implementation ---
template <
    typename MotorType,
    typename EncoderType,
    typename ConstantsType,
    typename PinConfigType>
class Motor
{
public:
    // Expose template parameters via using declarations
    using MotorType_t = MotorType;
    using EncoderType_t = EncoderType;
    using ConstantsType_t = ConstantsType;
    using PinConfigType_t = PinConfigType;

    // Constructor takes constants, pin configuration, and variadic args for encoder initialization
    template <typename T_Constants, typename T_PinConfig, typename... EncoderArgs>
    Motor(T_Constants &&constants, T_PinConfig &&pin_config, EncoderArgs &&...encoder_args)
        : constants_(std::forward<T_Constants>(constants)),
          pin_config_(std::forward<T_PinConfig>(pin_config))
          // Use aligned_storage + placement new for encoder if not NoEncoder
          ,
          encoder_storage_() // Allocate raw storage
    {
        // --- Constructor Body ---

        // Initialize pins based on PinConfigType - Same as before
        if constexpr (std::is_same_v<PinConfigType, OneOnlySpeed>)
        {
            pinMode(pin_config_.kSpeed, OUTPUT);
        }
        else if constexpr (std::is_same_v<PinConfigType, TwoSpeedDir>)
        {
            pinMode(pin_config_.kSpeed, OUTPUT);
            pinMode(pin_config_.kDir, OUTPUT);
        }
        else if constexpr (std::is_same_v<PinConfigType, ThreeSpeedDirEn>)
        {
            pinMode(pin_config_.kSpeed, OUTPUT);
            pinMode(pin_config_.kDir, OUTPUT);
            pinMode(pin_config_.kEn, OUTPUT);
            digitalWrite(pin_config_.kEn, LOW);
        }
        else if constexpr (std::is_same_v<PinConfigType, ThreeSpeedDirDir>)
        {
            pinMode(pin_config_.kSpeed, OUTPUT);
            pinMode(pin_config_.kDirA, OUTPUT);
            pinMode(pin_config_.kDirB, OUTPUT);
        }
        else if constexpr (std::is_same_v<PinConfigType, FourSpeedDirDirEn>)
        {
            pinMode(pin_config_.kSpeed, OUTPUT);
            pinMode(pin_config_.kDirA, OUTPUT);
            pinMode(pin_config_.kDirB, OUTPUT);
            pinMode(pin_config_.kEn, OUTPUT);
            digitalWrite(pin_config_.kEn, LOW);
        }
        else
        {
            static_assert(false, "Unsupported PinConfigType for pin initialization");
        }

        // --- Construct Encoder Member (using placement new) ---
        if constexpr (!std::is_same_v<EncoderType, NoEncoder>)
        {
            // Use perfect forwarding to pass the encoder_args to the EncoderType constructor
            new (&encoder_) EncoderType(std::forward<EncoderArgs>(encoder_args)...);
        }
        // NoEncoder case does nothing here
    }

    // Destructor to properly destroy the encoder if it was constructed
    ~Motor()
    {
        if constexpr (!std::is_same_v<EncoderType, NoEncoder>)
        {
            encoder_.~EncoderType(); // Explicitly call destructor
        }
    }

    // --- Initialization Method ---
    void Begin()
    {
        // Initialize pins (done in constructor, but can repeat/add here if needed)

        // Initialize encoder if it's not NoEncoder
        if constexpr (!std::is_same_v<EncoderType, NoEncoder>)
        {
            // Check if the encoder type has a 'Begin' method and call it
            if constexpr (std::is_same_v<EncoderType, QuadEncoderHandler>)
            {
                encoder_.Begin(); // Call the Begin method from QuadEncoderHandler
            }
            // Add calls for other specific encoder types if needed
        }

        // Initialize motor state (e.g., set initial speed to 0, ensure disabled if applicable)
        if constexpr (std::is_same_v<PinConfigType, ThreeSpeedDirEn> || std::is_same_v<PinConfigType, FourSpeedDirDirEn>)
        {
            digitalWrite(pin_config_.kEn, LOW); // Ensure motor is disabled initially
        }
        Stop(); // Set initial motor state to stopped
    }

    // --- Update Method ---
    void Update()
    {
        if constexpr (!std::is_same_v<EncoderType, NoEncoder>)
        {
            // Check if the encoder type has an 'Update' method and call it
            if constexpr (std::is_same_v<EncoderType, QuadEncoderHandler>)
            {
                encoder_.UpdateEnc(); // Call the UpdateEnc method from QuadEncoderHandler
            }
            // Add calls for other specific encoder types if needed
        }
        // Add motor-specific update logic here if any
    }

    // --- Motor Control Methods ---
    // Set the desired speed using the range [-MAXSPEEDINPUT, MAXSPEEDINPUT]
    void SetSpeed(float speed_value)
    {
        // Ensure speed is within the valid input range defined by constants
        speed_value = constrain(speed_value, -constants_.maxSpeedInput, constants_.maxSpeedInput);

        // Calculate the raw PWM command [-maxPWMOut, maxPWMOut]
        int motor_command = calculateMotorCommand(speed_value);

        // Apply the command using the specific pin configuration
        WriteSpeed(motor_command);
    }

    // Set the desired speed using a percentage [-100.0, 100.0]
    void SetSpeedPercentage(float speed_percentage)
    {
        // Ensure percentage is within the valid range [-100.0, 100.0]
        speed_percentage = constrain(speed_percentage, -100.0, 100.0);

        // Convert percentage to the internal speed value range
        float speed_value = (speed_percentage / 100.0) * constants_.maxSpeedInput;

        // Call the SetSpeed method with the calculated speed value
        SetSpeed(speed_value);
    }

    // Stop the motor
    void Stop()
    {
        WriteSpeed(0); // A simple stop is a 0 command
    }

    // Engage braking (implementation depends on driver/PinConfigType)
    void Brake()
    {
        if constexpr (std::is_same_v<PinConfigType, TwoSpeedDir>)
        {
            digitalWrite(pin_config_.kDir, HIGH);
            analogWrite(pin_config_.kSpeed, 0);
        }
        else if constexpr (std::is_same_v<PinConfigType, ThreeSpeedDirEn>)
        {
            digitalWrite(pin_config_.kEn, HIGH);
            digitalWrite(pin_config_.kDir, HIGH);
            analogWrite(pin_config_.kSpeed, 0);
        }
        else if constexpr (std::is_same_v<PinConfigType, ThreeSpeedDirDir>)
        {
            digitalWrite(pin_config_.kDirA, HIGH);
            digitalWrite(pin_config_.kDirB, HIGH);
            analogWrite(pin_config_.kSpeed, 0);
        }
        else if constexpr (std::is_same_v<PinConfigType, FourSpeedDirDirEn>)
        {
            digitalWrite(pin_config_.kEn, HIGH);
            digitalWrite(pin_config_.kDirA, HIGH);
            digitalWrite(pin_config_.kDirB, HIGH);
            analogWrite(pin_config_.kSpeed, 0);
        }
        else
        {
            Stop();
        }
    }

    // --- Encoder-related methods (conditionally available) ---
    template <typename T = EncoderType>
    auto getEncoderPosition() -> std::enable_if_t<!std::is_same_v<T, NoEncoder>, long> const // Added const
    {
        // Check encoder type to call the correct method name
        if constexpr (std::is_same_v<EncoderType, QuadEncoderHandler>)
        {
            return encoder_.GetEnc(); // Call GetEnc() from QuadEncoderHandler
        }
        else if constexpr (std::is_same_v<EncoderType, AnalogEncoder>)
        {
            return encoder_.getPosition(); // Call getPosition() from AnalogEncoder
        }
        else
        {
            static_assert(false, "getEncoderPosition not implemented for this EncoderType");
            return 0; // Should not be reached
        }
    }

    template <typename T = EncoderType>
    auto resetEncoder() -> std::enable_if_t<!std::is_same_v<T, NoEncoder>>
    {
        // Check if the encoder type has a 'reset' method and call it
        if constexpr (std::is_same_v<EncoderType, AnalogEncoder>)
        {
            encoder_.reset(); // Call reset() from AnalogEncoder
        }
        // Note: QuadEncoderHandler doesn't currently have a public reset method in your .cpp
        // Add logic here if you implement reset() in QuadEncoderHandler or other encoder types.
        else
        {
            static_assert(false, "resetEncoder not implemented for this EncoderType");
        }
    }

    // Method to acccess defined constants
    const ConstantsType &getConstants() const { return constants_; }
    const PinConfigType &getPinConfig() const { return pin_config_; }

private:
    ConstantsType constants_;  // Physical constants
    PinConfigType pin_config_; // Pin assignments and configuration

    // Use aligned_storage to hold the memory for the encoder object,
    typename std::aligned_storage<sizeof(EncoderType), alignof(EncoderType)>::type encoder_storage_;
    // Reference to the object constructed via placement new
    // Note: This reference is only valid AFTER the constructor's placement new call
    // and BEFORE the destructor's explicit destructor call.
    EncoderType &encoder_ = reinterpret_cast<EncoderType &>(encoder_storage_);

    // Helper method to calculate raw motor command [-maxPWMOut, maxPWMOut]
    // from a speed value [-maxSpeedInput, maxSpeedInput] using constants.
    int calculateMotorCommand(float speed_input) const
    {
        // Ensure maxSpeedInput and maxPWMOut are valid for scaling
        if constexpr (std::is_same_v<ConstantsType, RotationalMotorConstants> || std::is_same_v<ConstantsType, LinearMotorConstants>)
        {
            static_assert(ConstantsType::maxSpeedInput > 0, "maxSpeedInput must be greater than 0");
            static_assert(ConstantsType::maxPWMOut > 0, "maxPWMOut must be greater than 0"); // Assuming 8-bit PWM max
        }
        else
        {
            // If ConstantsType doesn't have these members, maybe use defaults or error
            static_assert(false, "ConstantsType must define maxSpeedInput and maxPWMOut");
        }

        // Scale the input speed to the PWM output range
        // Handles negative speeds and ensures output is within [-maxPWMOut, maxPWMOut]
        float scaled_command = (speed_input / constants_.maxSpeedInput) * static_cast<float>(constants_.maxPWMOut);

        // Return as int, clamping to the valid integer range [-maxPWMOut, maxPWMOut]
        return static_cast<int>(constrain(scaled_command, -static_cast<float>(constants_.maxPWMOut), static_cast<float>(constants_.maxPWMOut)));
    }

    // Helper method to apply the raw motor command [-maxPWMOut, maxPWMOut]
    // to the specific pins defined by PinConfigType.
    void WriteSpeed(int pwm_command)
    {
        // Use if constexpr to select pin control logic based on PinConfigType
        if constexpr (std::is_same_v<PinConfigType, OneOnlySpeed>)
        {
            // Control for one-pin config (e.g., simple PWM, likely unidirectional)
            // Assumes pwm_command sign might indicate direction if driver supports it implicitly,
            // or only positive commands are expected. Using abs() for simple PWM.
            analogWrite(pin_config_.kSpeed, std::abs(pwm_command)); // Use std::abs for int
        }
        else if constexpr (std::is_same_v<PinConfigType, TwoSpeedDir>)
        {
            // Control for two-pin config (e.g., PWM + Digital Direction)
            uint8_t pwm_value = std::abs(pwm_command); // Use std::abs for int
            if (pwm_command >= 0)
            {                                         // Positive command for forward
                digitalWrite(pin_config_.kDir, HIGH); // Adjust HIGH/LOW based on wiring
                analogWrite(pin_config_.kSpeed, pwm_value);
            }
            else
            {                                        // Negative command for reverse
                digitalWrite(pin_config_.kDir, LOW); // Adjust HIGH/LOW based on wiring
                analogWrite(pin_config_.kSpeed, pwm_value);
            }
        }
        else if constexpr (std::is_same_v<PinConfigType, ThreeSpeedDirEn>)
        {
            // Control for three-pin config (e.g., PWM + Digital Direction + Digital Enable)
            uint8_t pwm_value = std::abs(pwm_command); // Use std::abs for int
            if (pwm_value == 0)
            {
                digitalWrite(pin_config_.kEn, LOW); // Disable motor when command is 0
                analogWrite(pin_config_.kSpeed, 0);
            }
            else
            {
                digitalWrite(pin_config_.kEn, HIGH); // Enable motor
                if (pwm_command > 0)
                {                                         // Positive command for forward
                    digitalWrite(pin_config_.kDir, HIGH); // Adjust HIGH/LOW
                }
                else
                {                                        // Negative command for reverse
                    digitalWrite(pin_config_.kDir, LOW); // Adjust HIGH/LOW
                }
                analogWrite(pin_config_.kSpeed, pwm_value);
            }
        }
        else if constexpr (std::is_same_v<PinConfigType, ThreeSpeedDirDir>)
        {
            // Control for H-bridge style (PWM + two direction pins)
            uint8_t pwm_value = std::abs(pwm_command); // Use std::abs for int
            if (pwm_command == 0)
            {
                digitalWrite(pin_config_.kDirA, LOW);
                digitalWrite(pin_config_.kDirB, LOW); // Coast or brake depending on driver
                analogWrite(pin_config_.kSpeed, 0);
            }
            else if (pwm_command > 0)
            {                                          // Positive command for forward
                digitalWrite(pin_config_.kDirA, HIGH); // Forward
                digitalWrite(pin_config_.kDirB, LOW);
                analogWrite(pin_config_.kSpeed, pwm_value);
            }
            else
            { // Negative command for reverse
                digitalWrite(pin_config_.kDirA, LOW);
                digitalWrite(pin_config_.kDirB, HIGH); // Reverse
                analogWrite(pin_config_.kSpeed, pwm_value);
            }
        }
        else if constexpr (std::is_same_v<PinConfigType, FourSpeedDirDirEn>)
        {
            // Control for H-bridge style with Enable
            uint8_t pwm_value = std::abs(pwm_command); // Use std::abs for int
            if (pwm_value == 0)
            {
                digitalWrite(pin_config_.kEn, LOW); // Disable
                digitalWrite(pin_config_.kDirA, LOW);
                digitalWrite(pin_config_.kDirB, LOW);
                analogWrite(pin_config_.kSpeed, 0);
            }
            else
            {
                digitalWrite(pin_config_.kEn, HIGH); // Enable
                if (pwm_command > 0)
                {                                          // Positive command for forward
                    digitalWrite(pin_config_.kDirA, HIGH); // Forward
                    digitalWrite(pin_config_.kDirB, LOW);
                }
                else
                {                                          // Negative command for reverse
                    digitalWrite(pin_config_.kDirA, LOW);  // Reverse
                    digitalWrite(pin_config_.kDirB, HIGH); // Corrected logic
                }
                analogWrite(pin_config_.kSpeed, pwm_value);
            }
        }
        else
        {
            static_assert(false, "Unsupported PinConfigType for pin control");
        }
    }
};

#endif // MOTORTEMPLATE_H