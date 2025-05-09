/*
Motor_NFPGM25BL2838 - Specific implementation of motor template for this motor

Description:
Defines a concrete class that specializes generic motor template. Allows specifying encoder typedef

Edit history:
5/9/25 - Created class - Aldem Pido
*/
#ifndef MOTOR_NFPGM25BL2838
#define MOTOR_NFPGM25BL2838

#include "MotorTemplate.h"

template <typename EncoderType, typename... EncoderArgs>
class MOTOR_NFPGM25BL2838 : public Motor<RotationalMotor, EncoderType, RotationalMotorConstants, TwoSpeedDir>
{
public:
    using Base = Motor<RotationalMotor, EncoderType, RotationalMotorConstants, TwoSpeedDir>; // for convenience
    MOTOR_NFPGM25BL2838(const RotationalMotorConstants &constants,
                        const TwoSpeedDir &pin_config,
                        EncoderArgs &&...encoder_args)
        : Base(constants, pin_config, std::forward<EncoderArgs>(encoder_args)...), timeSinceReverse(0)
    {
        // Include any additional initialization here
    }

    // Note: any public methods from Base such as Begin, Update, SetSpeed, Stop, Break, etc... are automatically avialable

    void SetSpeed(float speed_value)
    {
        if (timeSinceReverse > 103000)
        {
            timeSinceReverse = 0;
            Base::SetSpeed(speed_value);
        }
        else if (timeSinceReverse > 100000)
        {
            int sign = (speed_value > 0.0f) - (speed_value < 0.0f);
            if (sign == 0)
            {
                sign = 1;
            }
            Base::SetSpeedPercentage(5.00f * static_cast<float>(sign));
        }
        else
        {
            Base::SetSpeed(speed_value);
        }
    }

private:
    elapsedMicros timeSinceReverse;
};

#endif
