// ex02 使用millis实现1Hz LED闪烁，周期1000ms，亮500ms灭500ms
const int ledPin = 2;
unsigned long preTime = 0;
const unsigned long interval = 500; // 500ms翻转一次，完整周期1000ms=1Hz
bool ledState = LOW;

void setup() {
  pinMode(ledPin, OUTPUT);
}

void loop() {
  unsigned long curTime = millis();
  if (curTime - preTime >= interval) {
    preTime = curTime;
    ledState = !ledState;
    digitalWrite(ledPin, ledState);
  }
}
