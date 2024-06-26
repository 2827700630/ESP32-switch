#include <Arduino.h>
#include <LittleFS.h>
// 在这里注释选择使用的库,在80行选择方向，在xfont.cpp35行选择方向
// #define ARDUINO_GFX
#define TFT_ESPI

#ifdef ARDUINO_GFX
#include "Arduino_GFX_Library.h"
#elif defined(TFT_ESPI)
#include <TFT_eSPI.h>
#endif
using std::wstring;

class XFont
{

public:
    XFont();

#ifdef ARDUINO_GFX

    Arduino_DataBus *bus = create_default_Arduino_DataBus();
    Arduino_GFX *tft = new Arduino_ST7735(bus);
#define GFX_BL DF_GFX_BL

#elif defined(TFT_ESPI)
    TFT_eSPI tft = TFT_eSPI();
#endif

    void DrawStr2(int x, int y, String str, int c);

    // 在指定位置输出中文
    void DrawChinese(int x, int y, String str, int c);

protected:
    void InitTFT();
    // 转化字符数组为字符串
    String getStringFromChars(uint8_t *bs, int l);
    // 把utf8编码字符转unicode编码
    String getUnicodeFromUTF8(String s);
    // 从字符的像素16进制字符重新转成二进制字符串
    String getPixDataFromHex(String s);

    void DrawSingleStr(int x, int y, String strBinData, int c, bool ansiChar);

    // 在屏幕上输出字符，暂时保留这个方法
    void DrawStr(int x, int y, String str, int color);

    // 初始化字库
    void initZhiku(String fontPath);
    // 从字库文件获取字符对应的编码字符串
    String getCodeDataFromFile(String strUnicode, String fontPath);
    // 从字库文件获取字符对应的二进制编码字符串
    String getPixBinStrFromString(String strUnicode, String fontPath);
    // 依照字号和编码方式计算每个字符存储展位
    int getFontPage(int font_size, int bin_type);

    // 所有字符的unicode编码
    String strAllUnicodes = "";
    int unicode_begin_idx = 0;
    int font_unicode_cnt = 0;
    int total_font_cnt = 0;
    int bin_type = 64;
    int font_size = 0;
    int font_page = 0;
    bool isInit = false;
    String fontFilePath = "/x.font";

    int pX = 16;
    int pY = 0;
    // int fontsize = 12;      // 字号
    int amountDisplay = 10; // 每行显示多少汉字，其实这个显示数量应该通过屏幕的宽度来计算字号
    // 下面的屏幕宽和高可以自行设定，在本处由于使用了tft屏幕驱动中的参数
    // int screenWidth = TFT_WIDTH;

#ifdef ARDUINO_GFX
    int screenHeight = 128;
    int screenWidth = 160;
#elif defined(TFT_ESPI)
    int screenHeight = TFT_HEIGHT;
    int screenWidth = TFT_WIDTH;
#endif

    // int screenHeight=128;
    int singleStrPixsAmount = 0;

private:
    bool chkAnsi(unsigned char c);
};
