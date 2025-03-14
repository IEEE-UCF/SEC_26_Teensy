#ifndef SUBSYSTEM_H
#define SUBSYSTEM_H

#include <Arduino.h>

class SortingSubsystem {
public:
  SortingSubsystem(int tosPin, int hallPin, int actuatorAPin, int actuatorBPin);
  void begin();
  void update();
  bool isFailsafeTriggered() const;
  void resetFailsafe();

private:
  enum State { IDLE, DEBOUNCING, SORTING };
  bool stableHallRead();

  State _currentState;
  unsigned long _lastUpdateTime;
  bool _lastTOSState;
  bool _isMagnetic;
  bool _failsafeTriggered;

  const int _tosPin, _hallPin, _actuatorAPin, _actuatorBPin;

  static constexpr unsigned long DEBOUNCE_DELAY_MS = 50;
  static constexpr unsigned long ACTUATOR_DURATION_MS = 500;
  static constexpr unsigned long GLOBAL_TIMEOUT_MS = 2000;
};

#endif
