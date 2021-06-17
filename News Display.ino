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

String urlEncode(String str)
{
    String encodedString="";
    char c;
    char code0;
    char code1;
    char code2;
    for (int i =0; i < str.length(); i++){
      c=str.charAt(i);
      if (c == ' '){
        encodedString+= '+';
      } else if (isalnum(c)){
        encodedString+=c;
      } else{
        code1=(c & 0xf)+'0';
        if ((c & 0xf) >9){
            code1=(c & 0xf) - 10 + 'A';
        }
        c=(c>>4)&0xf;
        code0=c+'0';
        if (c > 9){
            code0=c - 10 + 'A';
        }
        code2='\0';
        encodedString+='%';
        encodedString+=code0;
        encodedString+=code1;
      }
      yield();
    }
    return encodedString;
    
}

void downloadImage(String title, String imgURL, String  date, String author) {
    title = "title=" + urlEncode(title);
    imgURL = "&imgURL=" + imgURL;
    date = "&date=" + urlEncode(date);
    author = "&author=" + urlEncode(author);
    String url = "http://news-image-api.herokuapp.com/?" + title + imgURL + date + author;
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
    loadFile();
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

void loop() {
  // fs::Dir directory = SPIFFS.openDir("/");
  // while (directory.next()) {
  //   String strname = directory.fileName();
  //   // If filename ends in .jpg then load it
  //   if (strname.endsWith(".jpg")) {
  //   }
  // }
  // const char *name = '/.img.jpg';
  downloadImage(
    "NEO Surveyor: Space telescope to detect potentially dangerous asteroids heading for Earth - WION",
    "https://cdn.wionews.com/sites/default/files/styles/story_page/public/2021/06/17/198836-neo-surveyor.jpg",
    "2021-06-17",
    "WION Web Team"
  );
  delay(5000);
}
