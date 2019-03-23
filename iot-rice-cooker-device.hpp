#ifndef IOT_RICE_COOKER_DEVICE_HPP
#define IOT_RICE_COOKER_DEVICE_HPP

#include <ESP32Servo.h>
#include "web_client.hpp"

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
  LoadCellDout = 12,
  LoadCellSck  = 15,
  WaterSensor1 = 19,
  WaterSensor2 = 0,
  PressureSensor = 26,
  /* actuators */
  LidWireMotor1       = 17,
  LidWireMotor2       = 22,
  RiceDeliveryServo   = 13,
  RiceWashingMotor    = 1,
  RiceWashingRodServo = 5,
  WaterDeliveryPump   = 35,
  WaterRodServo       = 25,
  WaterSuctionPump    = 36,
};

enum StateId {
  STATE_STANDBY,
  STATE_OPENLID,
  STATE_POURWATER1,
  STATE_DROPRICE,
  STATE_WASHRICE,
  STATE_SUCKWATER,
  STATE_POURWATER2,
  STATE_CLOSELID,
  STATE_COOKING,
  STATE_COMPLETE,
};

struct State {
  StateId id = STATE_STANDBY;
  int amount = 0;
  bool water1 = true;
  double weight = 20;
};

class GearedMotor {
  public:
    void init(uint8_t _p1);
    void init(uint8_t _p1, uint8_t _p2);
    void forward();
    void reverse();
    void stop();
  private:
    uint8_t p1;
    uint8_t p2;
    void write(int v1, int v2);
};

String sendPutRequest(WebClient* client, String property, String value);

void sweepServo(Servo& servo, int from, int to, double speedDps = 90);

#endif // IOT_RICE_COOKER_DEVICE_HPP
