#include <Adafruit_GFX.h>   // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>

/***********************/
/***** LCD control *****/
/***********************/
#define TFT_CS       25
#define TFT_DC       26
#define TFT_MOSI     23
#define TFT_SCLK     18
#define TFT_RST      27 

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
bool isBlinking = false;

#define BLACK    0x0000
#define BLUE     0x001F
#define RED      0xF800
#define GREEN    0x07E0
#define CYAN     0x07FF
#define MAGENTA  0xF81F
#define YELLOW   0xFFE0 
#define WHITE    0xFFFF

void init_tft(){
  tft.init(172, 320);           // Init ST7789 172x320
  tft.setSPISpeed(40000000);
  tft.setRotation(1);
  tft.fillScreen(BLACK);
}

void setup(void) {
  
  init_tft();
  tft.fillScreen(BLACK);  //黒で塗りつぶす
  tft.fillCircle(50, 50, 40, GREEN);  //（向かって）左目
  tft.fillCircle(270, 50, 40, GREEN);  // 右目
  tft.fillCircle(160, 120, 40, GREEN);  // 鼻
}

void loop() {}

