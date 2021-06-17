#include <TJpg_Decoder.h>
#define FS_NO_GLOBALS
#include <FS.h>
#include "SPI.h"
#include <TFT_eSPI.h>


TFT_eSPI tft = TFT_eSPI();

// Display SDO/MISO to NodeMCU pin D6 (or leave disconnected if not reading TFT)
// Display LED to NodeMCU pin VIN (or 5V, see below)
// Display SCK to NodeMCU pin D5
// Display SDI/MOSI to NodeMCU pin D7
// Display DC (RS/AO)to NodeMCU pin D3
// Display RESET to NodeMCU pin D4 (or RST, see below)
// Display CS to NodeMCU pin D8 (or GND, see below)
// Display GND to NodeMCU pin GND (0V)
// Display VCC to NodeMCU 5V or 3.3V

bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap)
{
  if ( y >= tft.height() ) return 0;
  tft.pushImage(x, y, w, h, bitmap);
  return 1;
}


void loadFile()
{
  tft.fillScreen(TFT_BLACK);
  TJpgDec.drawFsJpg(0, 0, "/img.jpg");
}

void setup()
{
  Serial.begin(115200);
  Serial.println("\n\n Testing TJpg_Decoder library");

  // Initialise SPIFFS
  if (!SPIFFS.begin()) {
    Serial.println("SPIFFS initialisation failed!");
    while (1) yield();
  }
  Serial.println("\r\nInitialisation done.");

  // Initialise the TFT
  tft.begin();
  tft.setTextColor(0xFFFF, 0x0000);
  tft.fillScreen(TFT_BLACK);
  tft.setRotation(3); 


  // The jpeg image can be scaled by a factor of 1, 2, 4, or 8
  TJpgDec.setJpgScale(1);

  // The byte order can be swapped (set true for TFT_eSPI)
  TJpgDec.setSwapBytes(true);

  // The decoder must be given the exact name of the rendering function above
  TJpgDec.setCallback(tft_output);
}


void loop()
{
  // fs::Dir directory = SPIFFS.openDir("/");
  // while (directory.next()) {
  //   String strname = directory.fileName();
  //   // If filename ends in .jpg then load it
  //   if (strname.endsWith(".jpg")) {
  //   }
  // }
  // const char *name = '/.img.jpg';
  loadFile();
  delay(5000);
}
