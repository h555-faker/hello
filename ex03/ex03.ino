// ex03 SOS灯光：短亮150ms，长亮450ms，间隔150ms；整套结束停顿1000ms
const int ledPin = 2;
unsigned long preTime = 0;
unsigned long curTime;

// 时间定义
const unsigned long shortOn = 300;
const unsigned long longOn = 900;
const unsigned long gap = 250;     // 字符之间间隔
const unsigned long endGap = 1500;  // 整套SOS结束停顿

// SOS序列：0=短亮，1=长亮
int sosSeq[] = {0,0,0, 1,1,1, 0,0,0};
int seqIndex = 0;
bool waitEnd = false;

void setup() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
}

void loop() {
  curTime = millis();
  if (waitEnd) {
    if (curTime - preTime >= endGap) {
      waitEnd = false;
      seqIndex = 0;
      preTime = curTime;
    }
    return;
  }

  if (curTime - preTime >= (sosSeq[seqIndex] ? longOn : shortOn)) {
    digitalWrite(ledPin, LOW);
    preTime = curTime;
    delay(gap); // 字符间隔
    seqIndex++;
    if (seqIndex >= sizeof(sosSeq)/sizeof(int)) {
      waitEnd = true;
      preTime = curTime;
      return;
    }
    digitalWrite(ledPin, HIGH);
    preTime = curTime;
  }
}
