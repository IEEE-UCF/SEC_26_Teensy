/*
Motor_GenericHBridge - Example implementation for Rotational HBridge with Enable
Motor

Description:
Defines a concrete class that specializes generic motor template. Allows
specifying encoder typedef

Edit history:
5/9/25 - Created class - Aldem Pido
*/
#ifndef MOTOR_GENERICHBRIDGEENABLE
#define MOTOR_GENERICHBRIDGEENABLE
#include "MotorTemplate.h"

template <typename EncoderType, typename... EncoderArgs>
class MOTOR_NFPGM25BL2838
    : public Motor<RotationalMotor, EncoderType, RotationalMotorConstants,
                   FourSpeedDirDirEn> {
 public:
  using Base = Motor<RotationalMotor, EncoderType, RotationalMotorConstants,
                     FourSpeedDirDirEn>;  // for convenience
  Motor_GenericHBridgeEnable(const RotationalMotorConstants &constants,
                             const FourSpeedDirDirEn &pin_config,
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