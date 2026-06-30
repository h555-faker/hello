// 两路LED引脚定义
#define LED_A 18
#define LED_B 19

// PWM亮度变量 0~255
int brightness = 0;
// 亮度变化步长，控制渐变快慢
int step = 2;

void setup() {
  pinMode(LED_A, OUTPUT);
  pinMode(LED_B, OUTPUT);
}

void loop() {
  // A灯亮度：从0逐步增加到255
  analogWrite(LED_A, brightness);
  // B灯反相亮度：255 - brightness，同步反向变化
  analogWrite(LED_B, 255 - brightness);

  // 亮度递增
  brightness += step;

  // 到达边界后折返
  if (brightness >= 255 || brightness <= 0) {
    step = -step;
  }

  // 延时控制渐变顺滑度，数值越小切换越快
  delay(15);
}