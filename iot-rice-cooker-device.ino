#include <ESP32Servo.h>
#include <M5Stack.h>

Servo servo;

void setup() {
  M5.begin(); // setup serial, etc.
  delay(10);
  M5.Lcd.setFreeFont(&FreeMono9pt7b);
  M5.Lcd.setCursor(0,20);
  M5.Lcd.println("Hello, This is M5Stack!");
  /*** Sensors ***/
  servo.setPeriodHertz(50);
	servo.attach(21, 664, 2500); // TODO: 調整
  for(int c = 0; c < 100; c++) { // return to home position within 3 seconds
    servo.write(0);
    delay(30);
  }
}

void sweepServo(Servo& servo, int from, int to, double speedDps = 100.0) {
  int dir = (to - from) > 0 ? 1 : -1;
  for(double angle = from; (dir > 0 ? angle <= to : angle >= to); angle += speedDps * dir / 50) {
    M5.update();
    M5.Lcd.clear();
    M5.Lcd.setCursor(0,20);
    M5.Lcd.println("Hello, This is M5Stack!");
    M5.Lcd.println(round(angle));
    servo.write(round(angle));
    delay(20);
  }
}

void loop() {
  sweepServo(servo, 0, 180, 100);
  delay(500);
  sweepServo(servo, 180, 0, 200);
  delay(500);
}
