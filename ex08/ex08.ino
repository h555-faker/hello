#include <WiFi.h>
#include <WebServer.h>

// ========== 硬件引脚配置 ==========
#define TOUCH_PIN 4    // 触摸检测引脚T0(GPIO4)
#define ALARM_LED 2    // 报警LED引脚(板载LED)
int touchThreshold = 500;

// ========== AP热点配置（手机直连ESP32） ==========
const char* apSSID = "ESP32安防主机";
const char* apPWD  = "12345678";
WebServer server(80);

// ========== 全局安防状态变量（题目要求） ==========
bool isArm = false;    // false=撤防，true=布防
bool isAlarm = false;  // false=正常，true=报警锁定
// 触摸防抖变量
bool lastTouchFlag = false;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 60;

// 网页页面：布防/撤防按钮
String htmlPage = R"HTML(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>物联网安防报警器</title>
    <style>
        body {text-align:center;margin-top:80px;font-size:24px;}
        button {width:180px;height:70px;font-size:26px;margin:20px;cursor:pointer;}
        #armBtn {background:#c82423;color:white;}
        #disarmBtn {background:#28a745;color:white;}
        #stateText {font-size:30px;font-weight:bold;margin-top:40px;}
    </style>
</head>
<body>
    <h2>ESP32安防主机控制面板</h2>
    <button id="armBtn">布防 Arm</button>
    <button id="disarmBtn">撤防 Disarm</button>
    <div id="stateText">当前状态：未布防</div>

    <script>
        const armBtn = document.getElementById("armBtn");
        const disarmBtn = document.getElementById("disarmBtn");
        const stateText = document.getElementById("stateText");

        // 布防按钮点击
        armBtn.onclick = ()=>{
            fetch("/arm");
            stateText.innerText = "当前状态：已布防（触碰即报警）";
            stateText.style.color="red";
        }
        // 撤防按钮点击
        disarmBtn.onclick = ()=>{
            fetch("/disarm");
            stateText.innerText = "当前状态：未布防";
            stateText.style.color="#28a745";
        }
    </script>
</body>
)HTML";

// 主页路由：返回控制网页
void handleRoot(){
  server.send(200, "text/html", htmlPage);
}

// 布防接口
void handleArm(){
  isArm = true;
  server.send(200, "text/plain", "系统已布防");
}

// 撤防接口：同时清除报警状态、关闭LED
void handleDisarm(){
  isArm = false;
  isAlarm = false;
  digitalWrite(ALARM_LED, LOW);
  server.send(200, "text/plain", "系统已撤防，报警复位");
}

void setup() {
  Serial.begin(115200);
  pinMode(ALARM_LED, OUTPUT);
  digitalWrite(ALARM_LED, LOW);

  // 启动ESP32热点
  WiFi.softAP(apSSID, apPWD);
  IPAddress apIP = WiFi.softAPIP();
  Serial.print("安防主机热点：");
  Serial.println(apSSID);
  Serial.print("网页访问地址：");
  Serial.println(apIP);

  // 注册网页路由
  server.on("/", handleRoot);
  server.on("/arm", handleArm);
  server.on("/disarm", handleDisarm);
  server.begin();
  Serial.println("Web服务启动完成");
}

void loop() {
  server.handleClient(); // 持续处理网页请求

  // 1. 触摸检测+防抖边缘检测
  int touchVal = touchRead(TOUCH_PIN);
  bool currTouchFlag = (touchVal < touchThreshold);
  unsigned long now = millis();
  bool touchRisingEdge = (!lastTouchFlag) && currTouchFlag;

  // 2. 仅【布防状态】下，触摸触发锁定报警
  if(isArm && touchRisingEdge && (now - lastDebounceTime > debounceDelay)){
    lastDebounceTime = now;
    isAlarm = true; // 锁定报警，松手也不会取消
    Serial.println("检测到入侵，报警启动！");
  }
  lastTouchFlag = currTouchFlag;

  // 3. 报警状态：LED高频狂闪；未报警则熄灭
  if(isAlarm){
    digitalWrite(ALARM_LED, HIGH);
    delay(80);
    digitalWrite(ALARM_LED, LOW);
    delay(80);
  }else{
    digitalWrite(ALARM_LED, LOW);
  }
}