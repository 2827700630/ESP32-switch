 # 远程开机卡
 更多信息见https://oshwhub.com/myhym/yuan-cheng-kai-ji-ka-copy_2024-03-06_15-46-46

## 简介

本作品硬件上使用ESP32作为主控，软件上使用platformio编写程序，完成了远程开机卡的设计制作。可以在局域网内完成远程开机任务。

## 硬件

- ESP32-WROVER-I
- OLED显示屏
- 温度传感器


## 软件

- PlatformIO

## 功能

- 远程开机
- 网络控制台
- 温度显示
- 屏幕显示

## 使用方法

1. 将远程开机卡连接到电脑的USB接口。
2. 打开Arduino IDE，选择“工具”->“开发板”->“ESP32 Dev Module”。
3. 选择“工具”->“端口”->“COMX”，其中X是远程开机卡的端口号。
4. 打开“文件”->“打开”，选择远程开机卡的程序文件。
5. 点击“上传”按钮，将程序上传到远程开机卡。
6. 将远程开机卡连接到局域网内的路由器。
7. 在电脑的浏览器中输入远程开机卡的IP地址，即可访问网络控制台。

## 注意

- 远程开机卡需要连接到局域网内的路由器才能使用。
- 远程开机卡的IP地址可以在网络控制台中查看。
