#include <ESPAsyncWebServer.h> //网络控制台，需要安装ESPAsyncWebServer库
#include <WiFi.h>

#include <Adafruit_Sensor.h> //温湿度传感器
#include <DHT.h>

#include <TFT_eSPI.h> // TFT屏幕显示库，下载TFT_eSPI库后要在User_Setup.h中设置使用型号
#include <SPI.h>
#include "xfont.h" //中文显示，感谢https://github.com/StarCompute/tftziku/tree/master/src
XFont *_xFont;

#include <LittleFS.h> //使用LittleFS文件系统可以更好管理文件

#define LED_BUILTIN 2 // 板载蓝色LED灯
#define control 13    // 控制电脑开关

#define DHTPIN 4      // 温湿度传感器
#define DHTTYPE DHT11 // 型号DHT11
DHT dht(DHTPIN, DHTTYPE);

// 在这里设置AP热点名称和密码
const char *ssid = "ESP32远程开机卡";
const char *password = "12345678";

int switchmode = 0; // 开关状态，0为关，1为开

AsyncWebServer server(80);

TFT_eSPI tft = TFT_eSPI(); // TFT显示屏
void tftsetup()
{
  tft.init();                // TFT屏幕初始化
  tft.fillScreen(TFT_BLACK); // 设置背景为黑色
  // tft.setCursor(0, 0); // 鼠标置于屏幕原点
  tft.setTextColor(TFT_WHITE); // 设置字体颜色
  tft.setTextFont(4);          // 设置字体
  tft.initDMA();               // 开启DMA刷屏更快
}

void tftloop()
{
  /*
TFT_eSPI 库中的 tft.print() 和 tft.printf() 函数都用于在屏幕上打印文本，但它们有一些区别：
tft.print() 函数用于打印文本或变量的字符串表示形式。你可以使用它来打印数字、字符、字符串和其他数据类型的值。它可以接受不同类型的参数，并将它们转换为字符串进行显示。例如，tft.print(123) 将打印数字 "123"，tft.print("Hello") 将打印字符串 "Hello"。
tft.println()末尾换行
tft.printf() 函数是一个格式化打印函数，类似于C语言中的 printf()。它允许你使用格式化字符串和变量来构建输出。你可以在格式化字符串中包含特殊的格式说明符，如 %d（整数）、%f（浮点数）、%s（字符串）等，然后在参数列表中传递相应类型的变量。例如，tft.printf("Value: %d", 123) 将打印 "Value: 123"。
因此，主要区别在于 tft.printf() 允许你以更灵活和方便的方式构建输出，而 tft.print() 则更适合简单的文本打印和变量显示。
*/

  tft.fillScreen(TFT_BLACK); // 相当于清屏
  // tft.setCursor(0, 0); // 鼠标置于屏幕原点
  IPAddress myIP = WiFi.softAPIP();
  _xFont->DrawChinese(0, 0, "控制台", TFT_GREEN); // 在（0,0）开始显示汉字
  tft.setCursor(70, 0);                           // 鼠标置于（70,0）
  tft.println(myIP);                              // TFT屏幕上显示APIP地址
  tft.println(WiFi.localIP());                    // TFT屏幕上显示STAIP地址
  if (switchmode == 0)
  {
    _xFont->DrawChinese(0, 86, "当前:关机", TFT_YELLOW);
    Serial.print("当前:关机  ");
  }
  else if (switchmode == 1)
  {
    _xFont->DrawChinese(0, 86, "当前:开机", TFT_RED);
    Serial.print("当前:开机  ");
  }
}

void dhtloop()
{

  // 读取温度或湿度大约需要250毫秒！传感器读数可能也会延迟2秒（这是一个非常慢的传感器）。
  float h = dht.readHumidity();
  float t = dht.readTemperature();     // 读取摄氏温度（默认值）。
  float f = dht.readTemperature(true); // 读取华氏温度（isFahrenheit = true）。

  if (isnan(h) || isnan(t) || isnan(f)) // 检查是否有任何读取失败并提前退出（以便再次尝试）。
  {
    Serial.println(F("从DHT传感器读取失败！"));
    tft.println("读取失败");
    return;
  }

  float hif = dht.computeHeatIndex(f, h);        // 计算华氏温度下的热指数（默认值）。
  float hic = dht.computeHeatIndex(t, h, false); // 计算摄氏温度下的热指数（isFahreheit = false）。

  _xFont->DrawChinese(0, 110, "温度", TFT_BLUE);
  tft.setCursor(48, 110);
  tft.printf(":%.1fC", t);
  _xFont->DrawChinese(0, 134, "湿度", TFT_BLUE);
  tft.setCursor(48, 134);
  tft.printf(":%.1f%%", h);

  Serial.print(F("湿度："));
  Serial.print(h);
  Serial.print(F("%  温度："));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.print(f);
  Serial.print(F("°F  热指数："));
  Serial.print(hic);
  Serial.print(F("°C "));
  Serial.print(hif);
  Serial.println(F("°F"));
}

void setup()
{
  tftsetup();
  _xFont = new XFont();         // 中文显示
  pinMode(LED_BUILTIN, OUTPUT); // 设置GPIO输出模式
  pinMode(control, OUTPUT);

  Serial.begin(115200); // 串口波特率监测调整为115200，在platformio.ini中添加monitor_speed = 115200
  Serial.println();

  Serial.println("正在创建热点");
  WiFi.mode(WIFI_AP_STA);           // 设置WIFI模式
  if (!WiFi.softAP(ssid, password)) // 初始化WIFI
  {
    Serial.println("创建热点失败");
    while (1)
      ;
  }
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP 地址，请连接热点后进入这个网址以进入控制台: ");
  Serial.println(myIP);
  _xFont->DrawChinese(0, 0, "AP名称:ESP32远程开机卡，密码12345678，请连接热点后进入控制台 ", TFT_GREEN);

  if (!LittleFS.begin()) // 初始化LittleFS
  {
    Serial.println("LittleFS 挂载失败"); // 如果挂载失败输出错误信息
    return;                              // 返回，停止程序
  }

  server.begin();
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              request->send(LittleFS, "/home.html"); // 展示控制台界面
            });

  server.on("/getData", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              float temperature = dht.readTemperature();
              float humidity = dht.readHumidity();
              String json = "{\"temperature\":" + String(temperature) + ",\"humidity\":" + String(humidity) + "}";
              request->send(200, "application/json", json); // 上传JSON格式的传感器数据
            });

  server.on("/H", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              digitalWrite(LED_BUILTIN, HIGH);       // 亮灯
              request->send(LittleFS, "/home.html"); // 展示控制台界面
            });

  server.on("/L", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              digitalWrite(LED_BUILTIN, LOW);
              //  request->send(200, "text/html", "LED turned OFF");
              // 下面是返回控制台的操作，当然如/H那样直接再次发送"/home.html"也可以
              // 构造重定向的URL
              String home = "/";

              // 构造重定向的HTTP响应
              AsyncWebServerResponse *response = request->beginResponse(302, "text/plain", "Redirecting...");
              response->addHeader("Location", home);  // 添加重定向的目标URL
              request->send(response); });

  server.on("/setWifi", HTTP_POST, [](AsyncWebServerRequest *request)
            {
              String wifissid = request->arg("ssid");
              String wifipassword = request->arg("password");

              // 这里可以将WiFi名称和密码保存到ESP32的非易失性存储中(SD卡、flash)，以便以后使用

              Serial.println("WiFi名称和密码已接收,正在连接");
              Serial.print("WiFi名称:");
              Serial.println(wifissid);
              Serial.print("WiFi密码:");
              Serial.println(wifipassword);

              request->send(LittleFS, "/wait.html"); // 展示收到界面

              // WiFi.softAPdisconnect(true);//断开热点
              WiFi.begin(wifissid, wifipassword);

              while (WiFi.status() != WL_CONNECTED)
              {
                delay(500);
                Serial.print(".");
              }

              Serial.println("");
              Serial.println("WiFi连接成功.");
              Serial.println("IP地址: ");
              Serial.println(WiFi.localIP());
              // WiFi.softAPdisconnect(true);//断开热点
            });

  server.on("/ON", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              switchmode = 1;
              digitalWrite(control, HIGH);
              delay(100);
              digitalWrite(control, LOW);
              request->send(LittleFS, "/wait.html"); // 展示收到界面
            });

  server.on("/OFF", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              switchmode = 0;
              digitalWrite(control, HIGH);
              delay(100);
              digitalWrite(control, LOW);
              request->send(LittleFS, "/wait.html"); // 展示收到界面
            });

  Serial.println("控制台已启动");
  dht.begin();
  delay(1000);
}

void loop()
{
  tftloop();
  dhtloop();
  delay(2000);
}
