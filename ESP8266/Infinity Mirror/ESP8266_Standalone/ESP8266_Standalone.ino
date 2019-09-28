/**************************************************************
 * Blynk is a platform with iOS and Android apps to control
 * Arduino, Raspberry Pi and the likes over the Internet.
 * You can easily build graphic interfaces for all your
 * projects by simply dragging and dropping widgets.
 *
 *   Downloads, docs, tutorials: http://www.blynk.cc
 *   Blynk community:            http://community.blynk.cc
 *   Social networks:            http://www.fb.com/blynkapp
 *                               http://twitter.com/blynk_app
 *
 * Blynk library is licensed under MIT license
 * This example code is in public domain.
 *
 **************************************************************
 * This example runs directly on ESP8266 chip.
 *
 * You need to install this for ESP8266 development:
 *   https://github.com/esp8266/Arduino
 *
 * Please be sure to select the right ESP8266 module
 * in the Tools -> Board menu!
 *
 * Change WiFi ssid, pass, and Blynk auth token to run :)
 *
 **************************************************************/

//#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#define RedPin 5
#define GreenPin 13
#define BluePin 4


int power = 1;
int red = 0;
int green = 0;
int blue = 0;
float brightness = 1;
//animation variables
int ani = 0;
int color = 0;
int sped = 10;
int red1 = 0;
int green1 = 0;
int blue1 = 0;

void setup() {
  //Serial.begin(9600);
  Blynk.begin("b08d6e12c1864a7fb2f8fb2919c56920", "AmriFamilySmartHome", "WelcomeToTheFuture");
}

void loop() {
  Blynk.run();
    
  if (ani==1&&power==1) {
    if (color==0) {
      if (red1 < 1023) {
        red1++;
        analogWrite(RedPin, (red1)*brightness);
        analogWrite(BluePin, (1023-red1)*brightness);
        delay(sped);
      } else {
        color=1;
        red1=0;
      }
    }
    
    if (color==1) {
      if (blue1 < 1023) {
        blue1++;
        analogWrite(GreenPin, (blue1)*brightness);
        analogWrite(RedPin, (1023-blue1)*brightness);
        delay(sped);
      } else {
        color=2;
        blue1=0;
      }
    }
    
    if (color==2) {
      if (green1 < 1023) {
        green1++;
        analogWrite(BluePin, (green1)*brightness);
        analogWrite(GreenPin, (1023-green1)*brightness);
        delay(sped);
      } else {
        color=0;
        green1=0;
      }
    }
    
  }
  
}

//power button
BLYNK_WRITE(V0) {
  if (param.asInt()==0) {
    analogWrite(RedPin, 0);
    analogWrite(GreenPin, 0);
    analogWrite(BluePin, 0);
    power=0;
  } else {
    analogWrite(RedPin, red*brightness);
    analogWrite(GreenPin, green*brightness);
    analogWrite(BluePin, blue*brightness);
    power=1;
  }
}


//update led from sliders and zebra
BLYNK_WRITE(V1) { 
  red=param.asInt();   
  if (power==1&&ani==0) {analogWrite(RedPin, red*brightness);}   
}

BLYNK_WRITE(V2) { 
  green=param.asInt(); 
  if (power==1&&ani==0) {analogWrite(GreenPin, green*brightness);} 
}

BLYNK_WRITE(V3) { 
  blue=param.asInt();  
  if (power==1&&ani==0) {analogWrite(BluePin, blue*brightness);}  
}

BLYNK_WRITE(V4) { 
  red=param.asInt();   
  if (power==1&&ani==0) {analogWrite(RedPin, red*brightness);}   
}

BLYNK_WRITE(V5) { 
  green=param.asInt(); 
  if (power==1&&ani==0) {analogWrite(GreenPin, green*brightness);} 
}

BLYNK_WRITE(V6) { 
  blue=param.asInt();  
  if (power==1&&ani==0) {analogWrite(BluePin, blue*brightness);}  
}


//animation
BLYNK_WRITE(V7) {
  if (param.asInt()==0) {
    if (power==1) {
      analogWrite(RedPin, red*brightness);
      analogWrite(GreenPin, green*brightness);
      analogWrite(BluePin, blue*brightness);
    }
    ani = 0;
  } else {
    ani = 1;
  }
}

BLYNK_WRITE(V8) {
  sped=param.asInt();
}


//brightness
BLYNK_WRITE(V9) {
  brightness=param.asInt(); 
  brightness=brightness/100;
  if (power==1&&ani==0) {
    analogWrite(RedPin, red*brightness);
    analogWrite(GreenPin, green*brightness);
    analogWrite(BluePin, blue*brightness);
  }
}
