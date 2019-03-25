#include "iot-rice-cooker-device.hpp"

void GearedMotor::init(uint8_t _p1) {
  p1 = _p1;
  pinMode(p1, OUTPUT);
  digitalWrite(p1, 0);
}
void GearedMotor::init(uint8_t _p1, uint8_t _p2) {
  p1 = _p1;
  p2 = _p2;
  pinMode(p1, OUTPUT);
  digitalWrite(p1, LOW);
  pinMode(p2, OUTPUT);
  digitalWrite(p2, LOW);
}
void GearedMotor::forward() { write(1, 0); }
void GearedMotor::reverse() { write(0, 1); }
void GearedMotor::stop() { write(0, 0); }
void GearedMotor::write(int v1, int v2) {
  digitalWrite(p1, v1);
  if(p2) digitalWrite(p2, v2);
}

String sendPutRequest(WebClient* client, String property, String value) {
  return client->put_request(
    "/api/cookers/0/" + property,
    String("{\"") + property + String("\": \"") + value + String("\"}")
  );
}

void sweepServo(Servo& servo, int from, int to, double speedDps) {
  int dir = (to - from) > 0 ? 1 : -1;
  for(double angle = from; (dir > 0 ? angle <= to : angle >= to); angle += speedDps * dir / 18) {
    servo.write(round(angle));
    delay(20);
  }
}
