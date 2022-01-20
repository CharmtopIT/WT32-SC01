#include <FS.h>
#include <ESPAsyncWebServer.h>     //Local WebServer used to serve the configuration portal
#include <ESPAsyncWiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic
#include <pgmspace.h>
#include <ArduinoJson.h>
#include <Arduino.h>
#include <TFT_eSPI.h> // Hardware-specific library
#include <SPI.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include "Free_Fonts.h" // Include the header file attached to this sketch
#include <HTTPClient.h>
#define USE_SERIAL Serial
#include "set.h"
#include "sunny.h"
#include "rainy.h"
#include "cloudy.h"
#include "thunderstom.h"
//#include "set.h"
#include "co2.h"
#include <FT6236.h>
#include <esp_task_wdt.h>


WiFiMulti wifiMulti;
TFT_eSPI tft = TFT_eSPI();                   // Invoke custom library with default width and height
AsyncWebServer server(80);
DNSServer dns;
FT6236 ts = FT6236();

#define USE_SERIAL Serial
#define WDT_TIMEOUT 3
int query = 0;
unsigned long drawTime = 0;
int panduan = 0;

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
  if (!wifiManager.autoConnect("ESP-01", "aaaaaaaa")) {
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
    
  delay(1000); 

    USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();

    for(uint8_t t = 4; t > 0; t--) {
        USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
        USE_SERIAL.flush();
        delay(1000);
    }

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
        tft.pushImage(55, 80, setWidth, setHeight, set);
//        tft.setCursor(170, 80);
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
        
//        if ((p.x>=0 && p.y>390)){
//          delay(500);
//          
//          break;
//        }
}
}
  

void loop() {
  
    HTTPClient http1;
    HTTPClient http2;
  
    int xpos =  0;
    int ypos = 40;
    String wrain;
    String wtcsgnl;
    int index0;
    int index1;
    int index2;
    int index3;
    int index4;
    int xcoordinate = 20;
  
    http1.begin("https://q2slzz7s32.execute-api.eu-west-1.amazonaws.com/devlop/forestcastapi");
    int httpCode = http1.GET();
  
  
      if (httpCode > 0){
  
        
  
  //      delay(500);
  
  //      http1.begin("https://q2slzz7s32.execute-api.eu-west-1.amazonaws.com/devlop/forestcastapi");
        int http2Code = http1.GET();
  //      delay(500);
        String payload2 = http1.getString();
        char json2[500];
        payload2.toCharArray(json2, 600);
        USE_SERIAL.println(payload2);
        StaticJsonDocument<500> doc2;
        deserializeJson(doc2, json2);
        String s = doc2["Item"]["forecastDesc"];
  //      String x;
  //      if(s.length() >130){
  //        x=s.substring(0,45) + "-" + s.substring(45,91)+ "-" +s.substring(91,135) + "-" +s.substring(135) ;
  //      }else{
  //        x=s.substring(0,45) + "-" + s.substring(45,91)+ "-" +s.substring(91);
  //      }
        
        if(s.indexOf("Rainy") >= 0 || s.indexOf("rain") >= 0){
          Serial.println("rain");
          index1 = 1;
        }else{
          index1 = 0;
        }
        if(s.indexOf("winds") >= 0 || s.indexOf("windy") >= 0 || s.indexOf("cool") >= 0){
          Serial.println("winds");
          index2 = 1;
        }else{
          index2 = 0;
        }
        if(s.indexOf("Sunny") >= 0 || s.indexOf("sunny") >= 0 || s.indexOf("fine") >= 0 || s.indexOf("Fine") >= 0){
          Serial.println("sunny");
          index4 = 1;
        }else{
          index4 = 0;
        }
        if(s.indexOf("Cloudy") >= 0 || s.indexOf("cloudy") >= 0){
          Serial.println("Cloudy");
          index0 = 1;
          index4 = 0;
        }else{
          index0 = 0;
        }
        if(s.indexOf("thunder") >= 0 || s.indexOf("Thunder") >= 0){
          Serial.println("thunder");
          index3 = 1;
        }else{
          index3 = 0;
        }
        http1.end();
  //      delay(500);
  
        
        http1.begin("https://zmca4jf7ck.execute-api.eu-west-1.amazonaws.com/devlop/iaqi");
        int http3Code = http1.GET();
  //      delay(500);
        String payload3 = http1.getString();
        char json3[300];
        payload3.toCharArray(json3, 400);
        USE_SERIAL.println(payload3);
        StaticJsonDocument<300> doc3;
        deserializeJson(doc3, json3);
        int no2 = doc3["no2"];
        int pm25 = doc3["pm25"];
        int o3 = doc3["o3"];
        double uvi = doc3["uvi"];
        double so2 = doc3["so2"];
        int p = doc3["p"];
        int pm10 = doc3["pm10"];
        int aqi = doc3["aqi"];
        int temp1 = doc3["temperature"];
        int humi1 = doc3["humidity"];
        
  //      const char* forecastPeriod = doc2[0]["forecastPeriod"];
  //      const char* forecastDesc = doc2[0]["forecastDesc"];
  //      String temperature = doc2["Item"]["Temperature"];
  
  //      Serial.println(x);
  //      Serial.println(forecastDesc);
        http1.end();
  
        if (humi1 > 85){
            http1.begin("https://data.weather.gov.hk/weatherAPI/opendata/weather.php?dataType=warnsum&lang=en");
            delay(500);
            int http4Code = http1.GET();
  //          delay(1000);
            String payload4 = http1.getString();
            char json4[1000];
            payload4.toCharArray(json4, 1100);
            USE_SERIAL.println(payload4);
            StaticJsonDocument<1000> doc4;
            deserializeJson(doc4, json4);
            String warning = doc4["WRAIN"]["type"];
            String warning1 = doc4["WRAIN"]["name"];
            wrain =warning+" "+warning1;
            String wtcsgnl1 = doc4["WTCSGNL"]["type"];
            wtcsgnl =wtcsgnl1;
            USE_SERIAL.println(wtcsgnl);
            http1.end();
            delay(1000);
        }
        USE_SERIAL.println(wrain);
  
      
        
  
        if (temp1 != 0){
          
          tft.fillScreen(TFT_BLACK);
          tft.setTextColor(TFT_WHITE, TFT_BLACK);
          tft.setCursor(0, 40);
          tft.setTextFont(FONT4);
          tft.setFreeFont(FSB18);       // Select Free Serif 24 point font
          tft.print("Kwun Tong local weather: ");
          tft.println();                // Move cursor down a line
          tft.setFreeFont(FSB12);
          delay(500);
          tft.print("  Temperature:    ");
          delay(500);
          if(temp1 <= 26){
            tft.setTextColor(TFT_GREEN, TFT_BLACK);
            tft.print(temp1);
            tft.print(" C  ");
          }else{
            tft.setTextColor(TFT_YELLOW, TFT_BLACK);
            tft.print(temp1);
            tft.print(" C  ");
          }
  //        tft.print(temp);
          tft.setTextColor(TFT_WHITE, TFT_BLACK);
  //        tft.println();
          delay(500);
          tft.print("   Humidity:   ");
          tft.setTextColor(TFT_BLUE, TFT_BLACK);
          delay(500);
          tft.print(humi1);
          tft.print("%");
          tft.setTextColor(TFT_WHITE, TFT_BLACK);
  //        tft.print("  CO2:   ");
  //        if(co2 <= 800){
  //          tft.setTextColor(TFT_GREEN, TFT_BLACK);
  //          tft.print(co2);
  //          tft.print("ppm");
  //        }else{
  //          tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  //          tft.print(co2);
  //          tft.print("ppm");
  //        }
          
          tft.setFreeFont(FSB18);
          tft.setTextColor(TFT_WHITE, TFT_BLACK);
          tft.println();
          delay(1000);
          tft.print("Weather forecast for today:");
          tft.println();
          tft.setTextColor(TFT_GREEN, TFT_BLACK);
          tft.setFreeFont(FSB12);
          delay(1000);
  //        tft.print("   Mainly cloudy with a few showers.");
  //        tft.println();
  //        tft.print("   Showers gradually increasing with isolated");
  //        tft.println();
  //        tft.print("   thunderstorms tonight.");
  //        tft.println();
  //        tft.print("   Fresh east to northeasterly winds, strong");
  //        tft.println();
  //        tft.print("   offshore and on high ground.");
  //        tft.println();
  //        tft.print(x);
          tft.setSwapBytes(true);
          
          // Draw the icons
          if(index4 >= 1){
            delay(500);
            tft.pushImage(xcoordinate, 140, sunnyWidth, sunnyHeight, sunny);
            xcoordinate = xcoordinate + 80;
          }
          if(index0 == 1){
            delay(500);
            if(index1 == 1){
              delay(500);
              tft.pushImage(xcoordinate, 140, rainyWidth, rainyHeight, rainy);
              xcoordinate = xcoordinate + 80;
            }else{
              tft.pushImage(xcoordinate, 140, cloudyWidth, cloudyHeight, cloudy);
              xcoordinate = xcoordinate + 80;
            }
          }
          
          if(index2 == 1){
            delay(500);
            tft.pushImage(xcoordinate, 140, co2Width, co2Height, co2);
            xcoordinate = xcoordinate + 80;
          }
          if(index3 >= 1){
            delay(500);
            tft.pushImage(xcoordinate, 140, thunderWidth, thunderHeight, thunder);
            xcoordinate = xcoordinate + 80;
          }
          
          
//          tft.pushImage(180, 130, thunderWidth, thunderHeight, thunder);
//          tft.pushImage(260, 130, sunnyWidth, sunnyHeight, sunny);
          
//          tft.pushImage(410, 250, setWidth, setHeight, set);
          delay(500);
          tft.setTextColor(TFT_WHITE, TFT_BLACK);
          tft.setCursor(420, 300);
          tft.setTextFont(FONT4);
          tft.setFreeFont(FSB24);       // Select Free Serif 24 point font
          tft.print("->");
  
          int popo = 1;
          int count=0;
          while (popo){
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
          
  //        p.x=0;
  //        p.y=0;
  //        while (ts.touched() != 1)
  //        {
  //            
  ////            Serial.print(".");
  //            // Retrieve a point
  //            
  //    
  //            // Print coordinates to the serial output
  //            Serial.print("X Coordinate: ");
  //            Serial.println(p.x);
  //            Serial.print("Y Coordinate: ");
  //            Serial.println(p.y);
  //            delay(50);
  //        }
  
        
          // Pause here to admire the icons!
  //        delay(2000);
          tft.println();
          tft.setFreeFont(FSB12);
          if (wrain.length()>0){
            tft.setTextColor(TFT_RED, TFT_BLACK);
            tft.print(wrain);
            tft.print("  ");
            tft.print(wtcsgnl);
            tft.println();
          }
  //        tft.pushImage(360, 220, setWidth, setHeight, set);
//          delay(6000);
  
  
          tft.fillScreen(TFT_BLACK);
          tft.setTextColor(TFT_WHITE, TFT_BLACK);
          tft.setCursor(xpos, 40);
          tft.setTextFont(FONT4);
          tft.setFreeFont(FSB18);       // Select Free Serif 24 point font
          tft.print(" Kwun Tong AQI:  ");
          delay(1000);
          if(pm25 <= 50){
            tft.setTextColor(TFT_GREEN, TFT_BLACK);
            tft.print("Good");
  //          tft.pushImage(410, 250, setWidth, setHeight, set);
          }else{
            tft.setTextColor(TFT_YELLOW, TFT_BLACK);
            tft.print("Moderate");
  //          tft.pushImage(410, 250, setWidth, setHeight, set);
          }
          delay(1000);
  //        tft.setSwapBytes(true)
          
          tft.setTextColor(TFT_WHITE, TFT_BLACK);
          tft.println();                // Move cursor down a line
          tft.setFreeFont(FSB12);
          tft.print("  PM2.5:   ");
          if(pm25 <= 50){
            tft.setTextColor(TFT_GREEN, TFT_BLACK);
            tft.print(pm25);
          }else{
            tft.setTextColor(TFT_YELLOW, TFT_BLACK);
            tft.print(pm25);
          }
  //        tft.print(temp);
          tft.setTextColor(TFT_WHITE, TFT_BLACK);
          tft.println();
          delay(500);
          tft.print("  PM10:   ");
          tft.setTextColor(TFT_BLUE, TFT_BLACK);
          if(pm10 <= 50){
            tft.setTextColor(TFT_GREEN, TFT_BLACK);
            tft.print(pm10);
          }else{
            tft.setTextColor(TFT_YELLOW, TFT_BLACK);
            tft.print(pm10);
          }
          tft.setTextColor(TFT_WHITE, TFT_BLACK);
          tft.println();
          delay(500);
          tft.print("    O3:   ");
          tft.setTextColor(TFT_BLUE, TFT_BLACK);
          if(o3 <= 50){
            tft.setTextColor(TFT_GREEN, TFT_BLACK);
            tft.print(o3);
          }else{
            tft.setTextColor(TFT_YELLOW, TFT_BLACK);
            tft.print(o3);
          }
          tft.setTextColor(TFT_WHITE, TFT_BLACK);
          tft.println();
          delay(500);
          tft.print("   NO2:   ");
          tft.setTextColor(TFT_BLUE, TFT_BLACK);
          if(no2 <= 50){
            tft.setTextColor(TFT_GREEN, TFT_BLACK);
            tft.print(no2);
          }else{
            tft.setTextColor(TFT_YELLOW, TFT_BLACK);
            tft.print(no2);
          }
          tft.setTextColor(TFT_WHITE, TFT_BLACK);
          tft.println();
          delay(500);
          tft.print("   SO2:   ");
          tft.setTextColor(TFT_BLUE, TFT_BLACK);
          if(so2 <= 50){
            tft.setTextColor(TFT_GREEN, TFT_BLACK);
            tft.print(so2);
          }else{
            tft.setTextColor(TFT_YELLOW, TFT_BLACK);
            tft.print(so2);
          }
          tft.setTextColor(TFT_WHITE, TFT_BLACK);
          tft.println();
          delay(500); 
          tft.print("   UVI:   ");
          tft.setTextColor(TFT_BLUE, TFT_BLACK);
          if(uvi <= 50){
            tft.setTextColor(TFT_GREEN, TFT_BLACK);
            tft.print(uvi);
          }else{
            tft.setTextColor(TFT_YELLOW, TFT_BLACK);
            tft.print(uvi);
          }
//          tft.setTextColor(TFT_WHITE, TFT_BLACK);
          delay(500);
          tft.setTextColor(TFT_WHITE, TFT_BLACK);
          tft.setCursor(420, 300);
          tft.setTextFont(FONT4);
          tft.setFreeFont(FSB24);       // Select Free Serif 24 point font
//        tft.print("Reset");
        tft.drawString("Reset", 365, 275, GFXFF);
          while (popo){
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
            if(count>=160){
              Serial.println("Setting is touched! && count >=100");
              delay(500);
              break;
            }else if(p.x>=0 && p.y>390) {
              tft.fillScreen(TFT_BLUE);
              delay(1000);
              reset();
              break;
            }
            count = count + 1;
         }

        
      
      }
    }

  

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
