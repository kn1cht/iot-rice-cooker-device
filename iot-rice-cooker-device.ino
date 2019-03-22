#include <ESP32Servo.h>
#include <M5Stack.h>

Servo servo1;
Servo servo2;
Servo servo3;

void setup() {
  M5.begin(); // setup serial, etc.
  delay(10);
  M5.Lcd.setFreeFont(&FreeMono9pt7b);
  M5.Lcd.setCursor(0,20);
  M5.Lcd.println("Hello, This is M5Stack!");
  dac_output_disable( DAC_CHANNEL_1 );
  /*** Sensors ***/
  servo1.setPeriodHertz(50);
	servo1.attach(18, 664, 2500);
  servo2.setPeriodHertz(50);
	servo2.attach(19, 664, 2500);
  servo3.setPeriodHertz(50);
	servo3.attach(21, 664, 2500);
  for(int c = 0; c < 100; c++) { // return to home position within 3 seconds
    servo1.write(0);
    servo2.write(0);
    servo3.write(0);
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
  sweepServo(servo1, 0, 180, 100);
  delay(500);
  sweepServo(servo1, 180, 0, 200);
  delay(500);
}
