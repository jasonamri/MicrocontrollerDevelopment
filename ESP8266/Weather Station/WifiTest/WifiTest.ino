#include <ESP8266WiFi.h>
#include <WiFiClient.h>

void setup() {
  Serial.begin(9600);
  delay(50);

  WiFi.mode(WIFI_STA);
  WiFi.begin("AmriFamilyWifi", "Welcome2Wifi");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(50);
  }
    
  WiFiClient client;

  while (!client.connect("query.yahooapis.com", 80)) {
    delay(50);
  }
  
  client.print(String("GET ") + "/v1/public/yql?q=select%20*%20from%20weather.forecast%20where%20woeid%3D56616865%20and%20u%3D%27c%27 HTTP/1.1\r\nHost: query.yahooapis.com\r\nConnection: close\r\n\r\n");
  delay(50);
  
  
  Serial.println("Starting");
  while(client.available()){
    Serial.print(client.readStringUntil('<'));
    yield();
  }
  Serial.println("Ending");

}

void loop() {
  ESP.deepSleep(0);
}
