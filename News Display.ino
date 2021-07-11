#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <TJpg_Decoder.h>
#include <ArduinoJson.h>  // v6.18.0
#include <TFT_eSPI.h>
#include <FS.h>
#include "SPI.h"
#define FS_NO_GLOBALS


// Display SDO/MISO to NodeMCU pin D6 (or leave disconnected if not reading TFT)
// Display LED to NodeMCU pin VIN (or 5V, see below)
// Display SCK to NodeMCU pin D5
// Display SDI/MOSI to NodeMCU pin D7
// Display DC (RS/AO)to NodeMCU pin D3
// Display RESET to NodeMCU pin D4 (or RST, see below)
// Display CS to NodeMCU pin D8 (or GND, see below)
// Display GND to NodeMCU pin GND (0V)
// Display VCC to NodeMCU 5V or 3.3V

#define NEWS_INTERVAL 2000

TFT_eSPI tft = TFT_eSPI();
HTTPClient http;

const char* SSID = "ZTE_2.4G_ExQCMa";                 // Your WiFi SSID
const char* PASSWORD = "NullReferenceException#123";  // Your WiFi Password

const char* NEWS_URL = "https://newsapi.org/v2/top-headlines?country=in&apiKey=1fe4c0ba59564ef7aafa8e3831d1ad19";

String imageURLS;


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
  if (y >= tft.height()) return 0;
  tft.pushImage(x, y, w, h, bitmap);
  return 1;
}

void loadFile() {
  TJpgDec.drawFsJpg(0, 0, "/img.jpg");
}

void downloadImage(String imgURL) {
  String url = "http://news-image-api.herokuapp.com/get-image?imgURL=" + imgURL;
  Serial.println(url);
  File f = SPIFFS.open("/img.jpg", "w");
  if (f) {
    http.begin(url);
    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK) {
      http.writeToStream(&f);
      loadFile();
      delay(NEWS_INTERVAL);
    } else {
      Serial.printf("Getting images failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    f.close();
  }
  http.end();
}

void parseData() {

  if (!imageURLS) {
    return;
  }

  DynamicJsonDocument doc(3072);

  DeserializationError error = deserializeJson(doc, imageURLS);

  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  JsonArray arr = doc.as<JsonArray>();

  for (JsonVariant value : arr) {
    downloadImage(value.as<String>());
  }
}

void generateNews() {
  String url = "http://news-image-api.herokuapp.com/generate";
  String reqBody;
  StaticJsonDocument<128> doc;

  doc["endpoint"] = NEWS_URL;

  serializeJson(doc, reqBody);

  Serial.println(url);
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  int httpCode = http.POST(reqBody);

  if (httpCode == HTTP_CODE_OK) {
    imageURLS = http.getString();
  } else {
    imageURLS = "";
    Serial.printf("Generating images failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
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
  loadFile();  // show last downloaded image while new news is loading
}

void loop() {
  generateNews();
  parseData();
}
