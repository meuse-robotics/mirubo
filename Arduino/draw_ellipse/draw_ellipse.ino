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

void fillEllipse( uint16_t x, uint16_t y, uint16_t rx, uint16_t ry, int col )
{
  int8_t xt, x1, yt, y1, r;
  if (rx > ry) {
    xt=r=rx; yt=0;
    while (xt>=yt) {
      x1=xt*ry/rx;
      y1=yt*ry/rx;
      tft.drawFastHLine(x-xt, y+y1, 2*xt, col);
      tft.drawFastHLine(x-xt, y-y1, 2*xt, col);
      tft.drawFastHLine(x-yt, y+x1, 2*yt, col);
      tft.drawFastHLine(x-yt, y-x1, 2*yt, col);
      if ((r -= yt++ * 2 + 1) <= 0) r += --xt * 2;
    }
  } else {
    xt=r=ry; yt=0;
    while (xt>=yt) {
      x1=xt*rx/ry;
      y1=yt*rx/ry;
      tft.drawFastVLine(x+x1, y-yt, 2*yt, col);
      tft.drawFastVLine(x-x1, y-yt, 2*yt, col);
      tft.drawFastVLine(x+y1, y-xt, 2*xt, col);
      tft.drawFastVLine(x-y1, y-xt, 2*xt, col);
      if ((r -= yt++ * 2 + 1) <= 0) r += --xt * 2;
    }
  }
}

void setup(void) {
  init_tft();
  tft.fillScreen(BLACK);  //黒で塗りつぶす
  fillEllipse(50, 50, 30, 40, GREEN);  //（向かって）左目
  fillEllipse(50, 50, 20, 30, BLACK);
  fillEllipse(270, 50, 30, 40, GREEN);  // 右目
  fillEllipse(270, 50, 20, 30, BLACK);
  fillEllipse(160, 120, 60, 40, GREEN);  // 鼻
  fillEllipse(160, 120, 50, 30, BLACK);
}

void loop() {}

