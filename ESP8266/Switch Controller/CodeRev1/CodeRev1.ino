


//Libraries
#include <FS.h>
#include <Bounce2.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>


//#define BLYNK_PRINT Serial 
#define OUTPUT_PIN 2
#define SWITCH_PIN 3
#define ON_STATE 0

//Debounce object
Bounce debouncer = Bounce();

//AUTH TOKEN
char auth[] = "d21dbef54857427ea4ecd9c2bcaf2868";

//WIFI CREDS
char ssid[] = "AmriFamilySmartHome";
char *pass = "WelcomeToTheFuture";

//Pin status boolean
bool pinData;

//reconnection timer
unsigned long int reconnectTimer;


//connection function
void connectToCloud() {

  //connect to wifi if not connected
  if (WiFi.status() != WL_CONNECTED) {
    //setup creds
    WiFi.begin(ssid, pass);
    //start timeout timer
    long unsigned int timer = millis();
    //check for connection
    while (WiFi.status() != WL_CONNECTED) {
      //timeout handler
      if (timer+30000 < millis()) {break;}

      //handle switch in the meantime
      if (debouncer.update()) {
        pinData = !pinData; 
        digitalWrite(OUTPUT_PIN, pinData);
      }

      //don't trigger WDT
      delay(5);
    }
  }

  //connect to blynk server
  if (WiFi.status() == WL_CONNECTED && !Blynk.connected()) {
    Blynk.config(auth);
    Blynk.connect();
  }

  //handle a failed connection 
  if (Blynk.connected()) {
    //Serial message
    Serial.println("Connection Succeeded!");

    //update pinstate
    Blynk.virtualWrite(V0, ((pinData == ON_STATE) ? 1 : 0) );

  } else {
    //Serial message
    Serial.println("Connection Failed! Will reattempt in 5 mins");
    
    //reconnection timer
    reconnectTimer = millis();
  }
  
}

void setup()
{
  
  //Setup serial
  Serial.begin(9600,SERIAL_8N1,SERIAL_TX_ONLY); 

  //setup input
  pinMode(SWITCH_PIN, INPUT_PULLUP);
  debouncer.attach(SWITCH_PIN);
  debouncer.interval(25);
  
  //setup output (twice to prevent and funny business)
  pinData = !ON_STATE;
  digitalWrite(OUTPUT_PIN, pinData);
  pinMode(OUTPUT_PIN, OUTPUT);
  digitalWrite(OUTPUT_PIN, pinData);
  
  //connect
  connectToCloud();

}

void loop()
{
  //Blynk handler
  if (Blynk.connected()) {
    Blynk.run();
  } else if (reconnectTimer+300000<millis()) {
    connectToCloud();
  }

  //button change
  if (debouncer.update()) {
    Serial.println("Switch");
    pinData = !pinData; 
    digitalWrite(OUTPUT_PIN, pinData);
    if (Blynk.connected()) {
      Blynk.virtualWrite(V0, ((pinData == ON_STATE) ? 1 : 0) );
    }
  }
  
}


//update state
BLYNK_WRITE(V0) 
{
  pinData = param.asInt();

  //invert an off state switch
  if (ON_STATE == 0) {
    pinData = !pinData;
  }
  
  digitalWrite(OUTPUT_PIN, pinData);
}


