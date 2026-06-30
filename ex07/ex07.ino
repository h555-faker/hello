#include <WiFi.h>
#include <WebServer.h>

// ==================== AP热点配置（可自行修改名称密码）====================
const char* apSSID = "ESP32调光器";    // 手机搜到的WiFi名称
const char* apPWD  = "12345678";       // 热点连接密码（至少8位）
#define LED_PIN 2                       // ESP32板载LED引脚
// ======================================================================

WebServer server(80);
int ledBright = 0; // PWM亮度 0~255

// 网页页面：滑动条+JS fetch上传数值
String htmlPage = R"HTML(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32 热点无极调光</title>
    <style>
        body {text-align:center;font-size:22px;margin-top:60px;}
        #slider {width:85%;height:32px;margin:40px 0;}
        #showVal {font-size:30px;color:#ff3300;font-weight:bold;}
    </style>
</head>
<body>
    <h2>LED亮度调节滑块 0~255</h2>
    <!-- 题目要求滑动条 -->
    <input type="range" id="slider" min="0" max="255" value="0">
    <div>当前亮度：<span id="showVal">0</span></div>

    <script>
        let slider = document.getElementById("slider");
        let valText = document.getElementById("showVal");
        // 监听滑块拖动事件
        slider.addEventListener("input", function(){
            let num = this.value;
            valText.innerText = num;
            // fetch发送GET请求传递亮度数值
            fetch("/setLight?bright="+num);
        });
    </script>
</body>
)HTML";

// 主页路由：返回调光网页
void handleRoot(){
  server.send(200, "text/html", htmlPage);
}

// 接收滑块亮度参数路由
void handleSetLight(){
  if(server.hasArg("bright")){
    ledBright = server.arg("bright").toInt();
    analogWrite(LED_PIN, ledBright);
  }
  server.send(200, "text/plain", "OK");
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  analogWrite(LED_PIN, 0);

  // 启动AP热点模式
  Serial.print("正在创建热点：");
  Serial.println(apSSID);
  // AP固定IP：192.168.4.1（ESP32热点默认地址）
  WiFi.softAP(apSSID, apPWD);
  IPAddress apIP = WiFi.softAPIP();

  Serial.println("热点创建完成！");
  Serial.print("网页访问地址：");
  Serial.println(apIP);

  // 注册网页接口
  server.on("/", handleRoot);
  server.on("/setLight", handleSetLight);
  server.begin();
  Serial.println("Web服务器已运行，连接热点后浏览器输入上方IP调光");
}

void loop() {
  server.handleClient();
}