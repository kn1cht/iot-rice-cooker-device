#ifndef IOT_RICE_COOKER_DEVICE_HPP
#define IOT_RICE_COOKER_DEVICE_HPP

#include <ESP32Servo.h>
#include "web_client.hpp"
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

static const int SERVO_MIN_US = 664;
static const int SERVO_MAX_US = 2500;
static const uint8_t RICE_DELIVERY_HOME = 0;
static const uint8_t RICE_DELIVERY_DROP = 180;
static const uint8_t WATER_ROD_HOME = 180;
static const uint8_t WATER_ROD_DOWN = 45;
/* sensors' pins */
static const uint8_t LOAD_CELL_DOUT_PIN = 15;
static const uint8_t LOAD_CELL_SCK_PIN  = 12;
static const uint8_t WATER_TANK_SENSOR_PIN = 19;
static const uint8_t WASTE_TANK_SENSOR_PIN = 0;
// static const uint8_t PRESSURE_SENSOR_PIN = 26;
/* actuators' pins */
static const uint8_t LID_WIRE_MOTOR1_PIN = 5; //22→5
static const uint8_t LID_WIRE_MOTOR2_PIN = 17;
static const uint8_t RICE_DELIVERY_SERVO_PIN = 13;
static const uint8_t RICE_WASHING_MOTOR_PIN = 25;//21→25
//static const uint8_t RICE_WASHING_ROD_SERVO_PIN = 5; deleted
static const uint8_t WATER_DELIVERY_PUMP_PIN = 2;
//static const uint8_t WATER_ROD_SERVO_PIN = 25; deleted
static const uint8_t WATER_SUCTION_PUMP_PIN = 16;

//used by servo driver
static const int SERVO_MIN_US_DRIVER = 450;
static const int SERVO_MAX_US_DRIVER = 2030;
static const int SERVO_FREQ_DRIVER = 50;
static const int SERVO_RESOlUTION_DRIVER = 4096;
static const int CLOSER_SERVO_READY_ANGLE = 150;
static const int CLOSER_SERVO_CLOSE_ANGLE = 50;
static const int CLOSER_SERVO_NUM = 0;
static const int RICE_WASHING_SERVO_READY_ANGLE = 30;
static const int RICE_WASHING_SERVO_DOWN_ANGLE = 140;
static const int RICE_WASHING_SERVO_NUM = 1;
static const int WARTER_SERVO_READY_ANGLE = 180;
static const int WARTER_SERVO_DOWN_ANGLE = 75;
static const int WARETR_SERVO_NUM = 2;

//the threshold of rice shortage sensor
static const int RICE_SHORTAGE = 2270;

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

enum StateLifeCycle {
  INIT_STATE,
  MID_STATE,
  EXIT_STATE,
};

struct State {
  StateId id = STATE_STANDBY;
  StateLifeCycle lifeCycle = INIT_STATE;
  int amount = 0;
  bool water = true;
  bool waste = false;
  double weight = 0;
  double prevWeight = 0;
  double pressure = 0;
  bool isRiceShortage = false;
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
void sweepServoViaDriver(Adafruit_PWMServoDriver& pwm, int servoNum, int degree);

#endif // IOT_RICE_COOKER_DEVICE_HPP
