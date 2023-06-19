#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "MMA7660.h"
#include <Wire.h>
MMA7660 accelemeter;

//player ID
const String playerID = "23"; //n1 = team | n2 = playernum

//wifi connectie
const char* ssid = "sd-lab"; 
const char* password = "#2Geheim";
WiFiClientSecure client;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
   accelemeter.init();
  //wifi setup verbinden
  Serial.begin(4800);
  WiFi.begin(ssid, password);
  client.setInsecure();
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting..");
  }

  //cords setup dingetje
  Wire.begin(4800);
}

//function to get the speed
int getSpeedX(){
    int8_t x;
    int8_t y;
    int8_t z;
    float ax, ay, az;
    accelemeter.getXYZ(&x, &y, &z);
    Serial.println("x speed ");
    Serial.println(x);
    return x;
}

int getSpeedZ(){
    int8_t x;
    int8_t y;
    int8_t z;
    float ax, ay, az;
    accelemeter.getXYZ(&x, &y, &z);
    Serial.println("z speed ");
    Serial.println(z);
    return z;
}

//hier komt de stapscore multiplier
int beweegscore(float ax, float az) {
  if(ax < 0){
    ax = -ax;
  }
  else if(az < 0){
    az = -az;
  }
  int stapPunten = (int) (ax + az);
  return stapPunten;
}

int stapscore(int beweegpunten){
  if(beweegpunten > 25){
    if(beweegpunten > 50) {
      return 2;
    }
    return 1;
  }
  else {
    return 0;
  }
}

//function to send the speed to the server
int send2server(int stapPuntDef){
   if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(LED_BUILTIN, HIGH);
    String stapscoreStr = String(stapPuntDef);
    stapscoreStr.replace(" ", "");
    String link = "https://lasergame.stu.sd-lab.nl/stappen.php?spelspelerid=" + playerID + "&stapscore=" + stapscoreStr + "&authtoken=pantertje";
    Serial.println("url " + link);
    Serial.println("stapscore " + stapscoreStr);
    HTTPClient http;
    client.connect(link, '8080');
    http.begin(client, link);
    int httpCode = http.GET();
    
    if (httpCode > 0) {
      String payload = http.getString();
      Serial.println(payload);
    }
    else {
      Serial.println(httpCode);
    }
    http.end();
  }
}

void loop(){
  Serial.println("hoi");
  int ax = getSpeedX();
  int az = getSpeedZ();
  int iets1 = beweegscore(ax, az);
  delay(1000);
  int iets2 = beweegscore(ax, az);
  delay(1000);
  int iets3 = beweegscore(ax, az);
  int score = (iets1 + iets2 + iets3) / 3;
  if(stapscore(score) != 0){
      send2server(stapscore(score));
  }
  delay(1000);
  
}
