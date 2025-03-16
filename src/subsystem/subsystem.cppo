#include "subsystem.h"

// debugs macros with atomic timestamp
#ifdef DEBUG
  #define DEBUG_PRINT(msg) Serial.println(msg)
  #define DEBUG_TIMESTAMP() { Serial.print("[DEBUG] "); Serial.print(millis()); Serial.print(" ms: "); }
#else
  #define DEBUG_PRINT(msg)
  #define DEBUG_TIMESTAMP()
#endif

SortingSubsystem::SortingSubsystem(int tosPin, int hallPin, int actuatorAPin, int actuatorBPin)
  : _currentState(IDLE),
    _lastUpdateTime(0),
    _lastTOSState(false),
    _isMagnetic(false),
    _failsafeTriggered(false),
    _tosPin(tosPin),
    _hallPin(hallPin),
    _actuatorAPin(actuatorAPin),
    _actuatorBPin(actuatorBPin) {}

void SortingSubsystem::begin() {
  pinMode(_tosPin, INPUT);
  pinMode(_hallPin, INPUT);
  pinMode(_actuatorAPin, OUTPUT);
  pinMode(_actuatorBPin, OUTPUT);
  digitalWrite(_actuatorAPin, LOW);
  digitalWrite(_actuatorBPin, LOW);
}

bool SortingSubsystem::isFailsafeTriggered() const {
  return _failsafeTriggered;
}

void SortingSubsystem::resetFailsafe() {
  if (_failsafeTriggered && _currentState != SORTING) {
    _failsafeTriggered = false;
    _currentState = IDLE;
    digitalWrite(_actuatorAPin, LOW);
    digitalWrite(_actuatorBPin, LOW);
    DEBUG_TIMESTAMP(); DEBUG_PRINT("Failsafe reset - Ready");
  }
}

void SortingSubsystem::update() {
  unsigned long currentMillis = millis();

  // global failsafe (only triggers during DEBOUNCING)
  if (!_failsafeTriggered && 
      _currentState != IDLE && 
      _currentState != SORTING && 
      (currentMillis - _lastUpdateTime >= GLOBAL_TIMEOUT_MS)) {
    digitalWrite(_actuatorAPin, LOW);
    digitalWrite(_actuatorBPin, LOW);
    _currentState = IDLE;
    _failsafeTriggered = true;
    DEBUG_TIMESTAMP(); DEBUG_PRINT("Failsafe: Stuck in DEBOUNCING!!!!");
  }

  switch (_currentState) {
    case IDLE:
      if (_failsafeTriggered) break; // ignore inputs until reset

      bool currentTOS = digitalRead(_tosPin);
      if (currentTOS && !_lastTOSState) {
        DEBUG_TIMESTAMP(); DEBUG_PRINT("TOS trigger → Debouncing");
        _currentState = DEBOUNCING;
        _lastUpdateTime = currentMillis;
      }
      _lastTOSState = currentTOS;
      break;

    case DEBOUNCING:
      if (currentMillis - _lastUpdateTime >= DEBOUNCE_DELAY_MS) {
        if (digitalRead(_tosPin)) {
          _isMagnetic = stableHallRead();
          digitalWrite(_actuatorAPin, _isMagnetic ? HIGH : LOW);
          digitalWrite(_actuatorBPin, _isMagnetic ? LOW : HIGH);
          DEBUG_TIMESTAMP(); DEBUG_PRINT(_isMagnetic ? "Magnetic → Actuator A" : "Non-magnetic → Actuator B");
          _currentState = SORTING;
          _lastUpdateTime = currentMillis;
        } else {
          DEBUG_TIMESTAMP(); DEBUG_PRINT("Debounce failed");
          _currentState = IDLE;
        }
      }
      break;

    case SORTING:
      if (currentMillis - _lastUpdateTime >= ACTUATOR_DURATION_MS) {
        digitalWrite(_actuatorAPin, LOW);
        digitalWrite(_actuatorBPin, LOW);
        DEBUG_TIMESTAMP(); DEBUG_PRINT("sorting complete");
        _currentState = IDLE;
      }
      break;
  }
}

bool SortingSubsystem::stableHallRead() {
  constexpr uint8_t sampleCount = 5;
  uint8_t highCount = 0;
  unsigned long sampleStartTime = micros();

  for (uint8_t i = 0; i < sampleCount; i++) {
    highCount += digitalRead(_hallPin);
    
    // non-blocking time that is PRECISE!!!
    while (micros() - sampleStartTime < (i + 1) * 500) {
      yield(); // cooperative for RTOS
    }
  }

  return highCount >= (sampleCount / 2 + 1); // strict majority
}
