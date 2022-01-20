#include <FS.h>
#include <ESPAsyncWebServer.h>     //Local WebServer used to serve the configuration portal
#include <ESPAsyncWiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic
#include <pgmspace.h>
#include <ArduinoJson.h>
#include <Arduino.h>
#include <TFT_eSPI.h> // Hardware-specific library
#include <SPI.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>
#include <WiFiMulti.h>
#include "Free_Fonts.h" // Include the header file attached to this sketch
//#include "cert.h"
#include <FT6236.h>
#include <esp_task_wdt.h>
#define USE_SERIAL Serial

WiFiMulti wifiMulti;
TFT_eSPI tft = TFT_eSPI();                   // Invoke custom library with default width and height
AsyncWebServer server(80);
DNSServer dns;
FT6236 ts = FT6236();


unsigned long drawTime = 0;
#define USE_SERIAL Serial
#define WDT_TIMEOUT 3
int panduan = 0;
int count = 0;

String FirmwareVer = {
  "1.0"
};
#define URL_fw_Version "https://raw.githubusercontent.com/CharmtopIT/ESP32_OTAUPDATE/master/esp32_ota/bin_version.txt"
#define URL_fw_Bin "https://raw.githubusercontent.com/CharmtopIT/ESP32_OTAUPDATE/master/esp32_ota/fw.bin"

void connect_wifi();
void firmwareUpdate();
int FirmwareVersionCheck();

unsigned long previousMillis = 0;
unsigned long previousMillis_2 = 0;
const long interval = 60000;
const long mini_interval = 1000;

void repeatedCall() {
  static int num=0;
  unsigned long currentMillis = millis();
  if ((currentMillis - previousMillis) >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
    if (FirmwareVersionCheck()) {
      firmwareUpdate();
    }
  }
  if ((currentMillis - previousMillis_2) >= mini_interval) {
    previousMillis_2 = currentMillis;
    Serial.print("idle loop...");
    Serial.print(num++);
    Serial.print(" Active fw version:");
    Serial.println(FirmwareVer);
   if(WiFi.status() == WL_CONNECTED) 
   {
       Serial.println("wifi connected");
   }
   else
   {
    reset();
   }
  }
}

void reset(){
  esp_task_wdt_init(WDT_TIMEOUT, true);
  esp_task_wdt_add(NULL);
  while(1){
    delay(500);
  }
}

void setup(void) {

  tft.begin();

  tft.setRotation(1);
  USE_SERIAL.begin(115200);
  AsyncWiFiManager wifiManager(&server,&dns);
  wifiManager.setBreakAfterConfig(true);
//  wifiManager.resetSettings();
  if (!wifiManager.autoConnect("Ltgcs-ESP32", "aaaaaaaa")) {
      Serial.println("failed to connect, we should reset as see if it connects");
      delay(500);
//      hard_restart();
      delay(500);
  }
  if (WiFi.status() == WL_CONNECTED)
  {
      digitalWrite(2, HIGH);Serial.println("Connected to wifi");
      panduan=1;
  }
  Serial.print("Active firmware version:");
  Serial.println(FirmwareVer);
    
  delay(500); 

    USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();

    for(uint8_t t = 4; t > 0; t--) {
        USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
        USE_SERIAL.flush();
        delay(1000);
    }

//    WiFi.begin("CharmLinksys", "28511328");

if (!ts.begin(40, 18, 19))
    {
        Serial.println("Unable to start the capacitive touchscreen.");
    }

        tft.fillScreen(TFT_BLUE);
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.setCursor(110, 40);
        tft.setTextFont(GLCD);
        tft.setFreeFont(FSB18);       // Select Free Serif 24 point font
        tft.print("User Interface  ");
        tft.println();

        
        delay(500);


        tft.setTextColor(TFT_WHITE, TFT_BLACK);
          tft.setCursor(365, 310);
          tft.setTextFont(FONT4);
          tft.setFreeFont(FSB24);       // Select Free Serif 24 point font
          tft.print("WIFI"); 

        

        int count=0;
        
        tft.fillRect(40,40,145,200,TFT_BLUE);
        delay(1000);
//        tft.drawCircle(100, 100, 30,TFT_YELLOW ); 
//        tft.fillCircle(70, 270, 40,TFT_YELLOW);
//        tft.fillCircle(200, 100, 30,TFT_GREEN );
        while (1){
          TS_Point p = ts.getPoint();
          
          if(ts.touched() == 1){
            Serial.print("X Coordinate: ");
            Serial.println(p.x);
            Serial.print("Y Coordinate: ");
            Serial.println(p.y);
            delay(50);
          }else{
            delay(50);
          }
          if((p.x>=0 && p.y>390)){
            Serial.println("Setting is touched!");
            delay(50);
            tft.fillScreen(TFT_BLUE);
            tft.setTextColor(TFT_WHITE, TFT_BLACK);
            tft.setCursor(140, 40);
            tft.setTextFont(GLCD);
            tft.setFreeFont(FSB18);       // Select Free Serif 24 point font
            tft.print("WiFi Interface  ");
            tft.setCursor(170, 110);
            tft.print("RESET  ");
//            tft.setCursor(420, 300);
//            tft.setTextFont(FONT4);
//            tft.setFreeFont(FSB24);       // Select Free Serif 24 point font
//            tft.print("->");
            while (1){
              TS_Point p = ts.getPoint();
              
              if(ts.touched() == 1){
                Serial.print("X Coordinate: ");
                Serial.println(p.x);
                Serial.print("Y Coordinate: ");
                Serial.println(p.y);
                delay(50);
              }else{
                delay(50);
              }
              if((p.x>=170 && p.y>=230 && p.y<=260)){
                Serial.println("Setting is touched! && count >=100");
                delay(500);
                wifiManager.resetSettings();
                delay(2000);
                tft.fillScreen(TFT_BLUE);
                delay(500);
                reset();
                break;
              }else if(count >=140){
                break;
              }
              count = count + 1;
                
            }

        }
        count = count + 1;
        if(panduan==1){
          if ((p.x>=0 && p.y>390) || count>=140){
            break;
          }
        }
        

}

}

void loop() {

//  AsyncWiFiManager wifiManager(&server,&dns);
  
  HTTPClient http;
  HTTPClient http1;

  int xpos =  0;
  int ypos = 40;

  http1.begin("https://ha5eqvms3b.execute-api.eu-west-1.amazonaws.com/develop/lgcts");
  
  int httpCode = http1.GET();
  delay(500);

    if (httpCode > 0){
      String payload = http1.getString();
      char json[500];
      payload.toCharArray(json, 600);
      USE_SERIAL.println(payload);
      StaticJsonDocument<500> doc;
      deserializeJson(doc, json);
      int pm25 = doc["Item"]["PM25"];
      double nh3 = doc["Item"]["NH3"];
      double h2s = doc["Item"]["H2S"];
      double humidity = doc["Item"]["humidity"];
      double temperature = doc["Item"]["temperature"];
      String clock1 = doc["Item"]["datetime"];
      

//      Serial.print(temperature);
//      Serial.print(nh3);

        http1.end();
        delay(1000);
        http.begin("https://0nb9zeii09.execute-api.eu-west-1.amazonaws.com/hk/clock");
        int http3Code = http.GET();
        delay(500);
        String payload3 = http.getString();
        char json3[400];
        payload3.toCharArray(json3, 500);
        USE_SERIAL.println(payload3);
        StaticJsonDocument<400> doc3;
        deserializeJson(doc3, json3);
        int nowhour = doc3["hour"];
        int nowminute = doc3["minute"];
        String currentime = doc3["clock"];
        Serial.print(currentime);
    
      

      if (temperature != 0){
//        tft.fillScreen(TFT_BLACK); // Clear screen to navy background
  
  //  header("Draw free fonts using print class");
  
    // For comaptibility with Adafruit_GFX library the text background is not plotted when using the print class
    // even if we specify it.
//        tft.setTextColor(TFT_WHITE, TFT_BLACK);
//        tft.setCursor(xpos, ypos);    // Set cursor near top left corner of screen
//        tft.setTextFont(GLCD);     // Select the orginal small GLCD font by using NULL or GLCD
        tft.fillScreen(TFT_BLACK);
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.setCursor(xpos, ypos);
        tft.setTextFont(GLCD);
        tft.setFreeFont(FSB24);       // Select Free Serif 24 point font
        delay(500);
        tft.print(" Toilet IAQ ");
        tft.println();                // Move cursor down a line
        tft.setFreeFont(FSB18);
        delay(500);
        tft.print("Temperature:   ");
        delay(500);
        if(temperature <= 26){
          tft.setTextColor(TFT_GREEN, TFT_BLACK);
          tft.print(temperature);
          tft.print(" C");
        }else{
          tft.setTextColor(TFT_YELLOW, TFT_BLACK);
          tft.print(temperature);
          tft.print(" C");
        }
//        tft.print(temp);
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.println();
        delay(500);
        tft.print("Humidity:   ");
        tft.setTextColor(TFT_BLUE, TFT_BLACK);
        tft.print(humidity);
        tft.print("%");
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.println();
        delay(500);
        tft.print("NH3:   ");
        delay(500);
//        if(eco2 <= 800){
          tft.setTextColor(TFT_GREEN, TFT_BLACK);
          tft.print(nh3);
          tft.print("ppm");
//        }else{
//          tft.setTextColor(TFT_YELLOW, TFT_BLACK);
//          tft.print(nh3);
//          tft.print("ppm");
//        }
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.println();
        delay(500);
        tft.print("H2S:   ");
        tft.setTextColor(TFT_YELLOW, TFT_BLACK);
        delay(500);
        tft.print(h2s);
        tft.print("ppm");
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.println();
        delay(500);
        tft.print("PM2.5:    "); 
        delay(500);
        tft.setTextColor(TFT_GREEN, TFT_BLACK);
        tft.print(pm25);
        tft.print("ug/m3");
        tft.println();
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        delay(500);
        tft.print("Updated:    "); 
        delay(500);
        tft.print(currentime+"            ");
        delay(500);
        tft.setFreeFont(FSB12);
        tft.print("version " + FirmwareVer); 
          

        while (1){
            TS_Point p = ts.getPoint();
            
            if(ts.touched() == 1){
              Serial.print("X Coordinate: ");
              Serial.println(p.x);
              Serial.print("Y Coordinate: ");
              Serial.println(p.y);
              delay(50);
            }else{
              delay(50);
            }
            if(WiFi.status() == WL_CONNECTED){
              if((p.x>=0 && p.y>390) || count>=800){
                Serial.println("Setting is touched! && count >=100");
                delay(50);
                break;
              }
            }else{
              delay(500);
              reset();
            }
            
            count = count + 1;
          }
        
        delay(60000);
      
      }
    }
    


  delay(50000);

  

}



// Draw a + mark centred on x,y
void drawDatum(int x, int y)
{
  tft.drawLine(x - 5, y, x + 5, y, TFT_GREEN);
  tft.drawLine(x, y - 5, x, y + 5, TFT_GREEN);
}


#ifndef LOAD_GLCD
//ERROR_Please_enable_LOAD_GLCD_in_User_Setup
#endif

#ifndef LOAD_FONT2
//ERROR_Please_enable_LOAD_FONT2_in_User_Setup!
#endif

#ifndef LOAD_FONT4
//ERROR_Please_enable_LOAD_FONT4_in_User_Setup!
#endif

#ifndef LOAD_FONT6
//ERROR_Please_enable_LOAD_FONT6_in_User_Setup!
#endif

#ifndef LOAD_FONT7
//ERROR_Please_enable_LOAD_FONT7_in_User_Setup!
#endif

#ifndef LOAD_FONT8
//ERROR_Please_enable_LOAD_FONT8_in_User_Setup!
#endif

#ifndef LOAD_GFXFF
ERROR_Please_enable_LOAD_GFXFF_in_User_Setup!
#endif
