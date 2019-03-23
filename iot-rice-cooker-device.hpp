#ifndef IOT_RICE_COOKER_DEVICE_HPP
#define IOT_RICE_COOKER_DEVICE_HPP

#include <ESP32Servo.h>
#include <M5Stack.h>

static const int SERVO_MIN_US = 664;
static const int SERVO_MAX_US = 2500;
static const uint8_t RICE_DELIVERY_HOME = 0;
static const uint8_t RICE_DELIVERY_DROP = 180;
static const uint8_t RICE_WASHING_ROD_HOME = 0;
static const uint8_t RICE_WASHING_ROD_DOWN = 105;
static const uint8_t WATER_ROD_HOME = 180;
static const uint8_t WATER_ROD_DOWN = 75;

enum Pin : uint8_t {
  /* sensors */
  LoadCellDout = 23,
  LoadCellSck  = 24,
  WaterSensor1 = 2,
  WaterSensor2 = 5,
  /* actuators */
  LidWireMotor1       = 1,
  LidWireMotor2       = 3,
  RiceDeliveryServo   = 16,
  RiceWashingMotor    = 17,
  RiceWashingRodServo = 18,
  WaterDeliveryPump   = 19,
  WaterRodServo       = 21,
  WaterSuctionPump    = 22,
};

enum StateId {
  STATE_OPENLID,
  STATE_WATER_1,
  STATE_DROPRICE,
  STATE_WASHRICE,
  STATE_WATER_2,
  STATE_CLOSELID,
  STATE_COMPLETE,
  STATE_STANDBY,
}

struct State{
  StateId id    = STATE_STANDBY;
  bool water1   = true;
  double weight = 20;
};

class GearedMotor {
  public:
    void init(uint8_t _p1) {
      p1 = _p1;
      pinMode(p1, OUTPUT);
    }
    void init(uint8_t _p1, uint8_t _p2) {
      p1 = _p1;
      p2 = _p2;
      pinMode(p1, OUTPUT);
      pinMode(p2, OUTPUT);
    }
    void forward() { write(1, 0); }
    void reverse() { write(0, 1); }
    void stop() { write(0, 0); }
  private:
    uint8_t p1;
    uint8_t p2;
    void write(int v1, int v2) {
      digitalWrite(p1, v1);
      if(p2) digitalWrite(p2, v2);
    }
};

#endif // IOT_RICE_COOKER_DEVICE_HPP
