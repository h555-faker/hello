#include <WiFi.h>
#include <WebServer.h>

// ========== 硬件引脚配置 ==========
#define TOUCH_PIN 4    // T0对应GPIO4触摸引脚
// ========== ESP32热点配置 ==========
const char* apSSID = "ESP32触摸仪表盘";
const char* apPWD  = "12345678";
WebServer server(80);

// 存储实时触摸原始数值
int touchRawVal = 0;

// 仪表盘HTML页面（AJAX轮询实时刷新数据）
String dashHtml = R"HTML(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>触摸传感器实时仪表盘</title>
    <style>
        body{text-align:center;background:#0f172a;color:#fff;margin-top:80px;font-family:Arial;}
        h2{font-size:32px;opacity:0.9;}
        #dataBox{font-size:100px;font-weight:bold;color:#00e676;margin:50px 0;letter-spacing:4px;}
        .tip{font-size:20px;color:#aaa;}
    </style>
</head>
<body>
    <h2>触摸传感器实时采集数值</h2>
    <div id="dataBox">--</div>
    <div class="tip">手指靠近引脚 → 数值变小 | 手指离开 → 数值恢复变大</div>

    <script>
        // 定时轮询函数，每100ms拉取一次传感器数据
        function updateSensorData(){
            fetch("/getTouchData")
            .then(res => res.text())
            .then(val => {
                document.getElementById("dataBox").innerText = val;
            })
            .catch(err => console.log("数据获取失败"));
        }
        // 每100毫秒自动刷新
        setInterval(updateSensorData, 100);
        // 页面加载立刻读取一次
        updateSensorData();
    </script>
</body>
)HTML";

// 路由1：返回仪表盘网页
void handleRoot(){
  server.send(200, "text/html", dashHtml);
}

// 路由2：数据接口，仅返回当前触摸原始数值（AJAX拉取专用）
void handleGetTouch(){
  touchRawVal = touchRead(TOUCH_PIN);
  server.send(200, "text/plain", String(touchRawVal));
}

void setup() {
  Serial.begin(115200);
  // 开启AP热点
  WiFi.softAP(apSSID, apPWD);
  IPAddress apIP = WiFi.softAPIP();
  Serial.print("热点名称：");
  Serial.println(apSSID);
  Serial.print("网页仪表盘访问地址：");
  Serial.println(apIP);

  // 注册网页与数据接口路由
  server.on("/", handleRoot);
  server.on("/getTouchData", handleGetTouch);
  server.begin();
  Serial.println("Web仪表盘服务启动完成");
}

void loop() {
  server.handleClient();
}