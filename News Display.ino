#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <TJpg_Decoder.h>
#define FS_NO_GLOBALS
#include <FS.h>
#include "SPI.h"
#include <TFT_eSPI.h>


TFT_eSPI tft = TFT_eSPI();
HTTPClient http;

const char* SSID = "ZTE_2.4G_ExQCMa";                 // Your WiFi SSID
const char* PASSWORD = "NullReferenceException#123";  // Your WiFi Password

// Display SDO/MISO to NodeMCU pin D6 (or leave disconnected if not reading TFT)
// Display LED to NodeMCU pin VIN (or 5V, see below)
// Display SCK to NodeMCU pin D5
// Display SDI/MOSI to NodeMCU pin D7
// Display DC (RS/AO)to NodeMCU pin D3
// Display RESET to NodeMCU pin D4 (or RST, see below)
// Display CS to NodeMCU pin D8 (or GND, see below)
// Display GND to NodeMCU pin GND (0V)
// Display VCC to NodeMCU 5V or 3.3V


void setupTFT() {
  tft.begin();
  tft.setTextColor(0xFFFF, 0x0000);
  tft.fillScreen(TFT_BLACK);
  tft.setRotation(3);

  TJpgDec.setJpgScale(1);
  TJpgDec.setSwapBytes(true);
  TJpgDec.setCallback(tft_output);
}

void setupWiFi() {
  tft.setTextColor(ILI9341_YELLOW);
  tft.setTextSize(2);
  Serial.print("\nConnecting...");
  tft.print("\nConnecting...");

  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    tft.print(".");
    delay(500);
  }
  Serial.print("\nConnected - ");
  Serial.println(WiFi.localIP());
  tft.print("\nConnected - ");
  tft.println(WiFi.localIP());
}


bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap) {
  if ( y >= tft.height() ) return 0;
  tft.pushImage(x, y, w, h, bitmap);
  return 1;
}


void loadFile() {
  tft.fillScreen(TFT_BLACK);
  TJpgDec.drawFsJpg(0, 0, "/img.jpg");
}

void setup() {
  Serial.begin(115200);

  if (!SPIFFS.begin()) {
    Serial.println("SPIFFS initialisation failed!");
    while (1) yield();
  }
  Serial.println("SPIFFS Initialisation done.");

  setupTFT();
  setupWiFi();
}

void downloadImage() {
    String url = "http://news-image-api.herokuapp.com/";
    Serial.println(url);
    File f = SPIFFS.open("/img.jpg", "w");
    if (f) {
      http.begin(url);
      int httpCode = http.GET();
      if (httpCode > 0) {
        if (httpCode == HTTP_CODE_OK) {
          http.writeToStream(&f);
        }
      } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }
      f.close();
    }
    http.end();
}

void loop() {
  // fs::Dir directory = SPIFFS.openDir("/");
  // while (directory.next()) {
  //   String strname = directory.fileName();
  //   // If filename ends in .jpg then load it
  //   if (strname.endsWith(".jpg")) {
  //   }
  // }
  // const char *name = '/.img.jpg';
  downloadImage();
  loadFile();
  delay(5000);
}
