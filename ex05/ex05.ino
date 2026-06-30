// 引脚定义
#define TOUCH_PIN 4
#define LED_PIN 2
int threshold = 500;

// 触摸防抖、边缘检测变量
bool lastTouchFlag = false;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 60;

// 呼吸灯档位与参数
int speedGear = 1;   // 初始1档，档位：1/2/3
int ledDuty = 0;     // PWM占空比 0~255
int dir = 1;         // 亮度变化方向 1变亮，-1变暗

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  // 1. 触摸检测 + 边缘防抖切换档位
  int touchVal = touchRead(TOUCH_PIN);
  bool currTouchFlag = (touchVal < threshold);
  unsigned long now = millis();

  // 上升沿：未触摸 → 触摸瞬间
  bool touchTrigger = (!lastTouchFlag) && currTouchFlag;
  if (touchTrigger && (now - lastDebounceTime) > debounceDelay) {
    lastDebounceTime = now;
    // 档位循环 1→2→3→1
    speedGear++;
    if (speedGear > 3) speedGear = 1;
    Serial.print("切换档位：");
    Serial.println(speedGear);
  }
  lastTouchFlag = currTouchFlag;

  // 2. 根据档位设置呼吸步长（控制快慢）
  int step;
  switch(speedGear) {
    case 1: step = 1; break;  // 慢呼吸
    case 2: step = 3; break;  // 中速
    case 3: step = 6; break;  // 急促快速呼吸
  }

  // 3. PWM呼吸渐变逻辑
  ledDuty += dir * step;
  analogWrite(LED_PIN, ledDuty);

  // 到达极值反转方向
  if (ledDuty >= 255) dir = -1;
  if (ledDuty <= 0) dir = 1;

  // 微小延时控制基础刷新频率
  delay(10);
}