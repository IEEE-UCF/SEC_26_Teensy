/*
Motor_GenericHBridge - Example implementation for Linear HBridge Motor

Description:
Defines a concrete class that specializes generic motor template. Allows
specifying encoder typedef

Edit history:
5/9/25 - Created class - Aldem Pido
*/
#ifndef MOTOR_GENERICLINEARHBRIDGE
#define MOTOR_GENERICLINEARHBRIDGE
#include "MotorTemplate.h"

template <typename EncoderType, typename... EncoderArgs>
class MOTOR_NFPGM25BL2838
    : public Motor<LinearMotor, EncoderType, LinearMotorConstants,
                   ThreeSpeedDirDir> {
 public:
  using Base = Motor<LinearMotor, EncoderType, LinearMotorConstants,
                     ThreeSpeedDirDir>;  // for convenience
  Motor_GenericHBridge(const LinearMotorConstants &constants,
                       const ThreeSpeedDirDir &pin_config,
                       EncoderArgs &&...encoder_args)
      : Base(constants, pin_config,
             std::forward<EncoderArgs>(encoder_args)...) {
    // Include any additional initialization here
  }

  // Note: any public methods from Base such as Begin, Update, SetSpeed, Stop,
  // Break, etc... are automatically avialable Overwrite any template methods
  // here
 private:
};

#endif