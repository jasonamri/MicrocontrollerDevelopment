//Libraries
#include <FS.h>
#include <Bounce2.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#include <StreamString.h>


#define OUTPUT_PIN 2
#define SWITCH_PIN 3
#define ON_STATE 0

#define MyApiKey "7ab2281f-c93e-4415-84b1-fee30b1e8b3b" // TODO: Change to your sinric API Key. Your API Key is displayed on sinric.com dashboard
#define MySSID "AmriFamilySmartHome" // TODO: Change to your Wifi network SSID
#define MyWifiPassword "WelcomeToTheFuture" // TODO: Change to your Wifi network password

#define HEARTBEAT_INTERVAL 300000 // 5 Minutes 

#define DEVICE "5bc7ae5698b169383ae6c676"  //TODO: Device ID of first device


ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;
WiFiClient client;
Bounce debouncer = Bounce();


//Pin status boolean
bool pinData;


uint64_t heartbeatTimestamp = 0;
bool isConnected = false;


void setPowerStateOnServer(String deviceId, String value);


void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      isConnected = false;    
      Serial.printf("[WSc] Webservice disconnected from sinric.com!\n");
      break;
    case WStype_CONNECTED: {
      isConnected = true;
      Serial.printf("[WSc] Service connected to sinric.com at url: %s\n", payload);
      Serial.printf("Waiting for commands from sinric.com ...\n");        
      }
      break;
    case WStype_TEXT: {
        Serial.printf("[WSc] get text: %s\n", payload);
          
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject((char*)payload); 
        String deviceId = json ["deviceId"];     
        String action = json ["action"];
        
        if(action == "action.devices.commands.OnOff" && deviceId == DEVICE) { // Switch or Light
            String value = json ["value"]["on"];
            if(value=="true") {
               pinData = 1;
            } else {
               pinData = 0;
            }

            if (ON_STATE == 0) {
              pinData = !pinData;
            }

            digitalWrite(OUTPUT_PIN, pinData);
 
        }
        else if (action == "test") {
            Serial.println("[WSc] received test command from sinric.com");
        }
      }
      break;
    case WStype_BIN:
      Serial.printf("[WSc] get binary length: %u\n", length);
      break;
  }
}

void setup() {
  //Setup serial
  Serial.begin(9600,SERIAL_8N1,SERIAL_TX_ONLY); 

  //setup input
  pinMode(SWITCH_PIN, INPUT_PULLUP);
  debouncer.attach(SWITCH_PIN);
  debouncer.interval(25);
    
  //setup output (twice to prevent any funny business)
  //start "off"
  pinData = !ON_STATE;
  digitalWrite(OUTPUT_PIN, pinData);
  pinMode(OUTPUT_PIN, OUTPUT);
  digitalWrite(OUTPUT_PIN, pinData);
  
  WiFiMulti.addAP(MySSID, MyWifiPassword);
  Serial.println();
  Serial.print("Connecting to Wifi: ");
  Serial.println(MySSID);  

  // Waiting for Wifi connect
  while(WiFiMulti.run() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  if(WiFiMulti.run() == WL_CONNECTED) {
    Serial.println("");
    Serial.print("WiFi connected. ");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }

  // server address, port and URL
  webSocket.begin("iot.sinric.com", 80, "/");

  // event handler
  webSocket.onEvent(webSocketEvent);
  webSocket.setAuthorization("apikey", MyApiKey);
  
  // try again every 5000ms if connection has failed
  webSocket.setReconnectInterval(5000);   // If you see 'class WebSocketsClient' has no member named 'setReconnectInterval' error update arduinoWebSockets
}

void loop() {
  webSocket.loop();

  if (debouncer.update()) {
    pinData = !pinData; 
    digitalWrite(OUTPUT_PIN, pinData);
  }

  
  if(isConnected) {
      uint64_t now = millis();
      
      // Send heartbeat in order to avoid disconnections during ISP resetting IPs over night. Thanks @MacSass
      if((now - heartbeatTimestamp) > HEARTBEAT_INTERVAL) {
          heartbeatTimestamp = now;
          webSocket.sendTXT("H");          
      }
  }   
}
