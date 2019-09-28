//sleep
#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include "Arduino.h"
volatile bool sleep = false;
volatile bool f_wdt=1;
volatile int refresh = 0;

//lcd
#include <LiquidCrystal.h>
byte degree[8] = {
  B11100,
  B10100,
  B11100,
  B00000,
  B00000,
  B00000,
  B00000,
};

//weather data
int chill;
int humidity;
String sunrise;
String sunset;
int code[11];
int temp;
String text[11];
String date[11];
String Day[11];
int high[11];
int low[11];

//lcd
LiquidCrystal lcd(A4, A3, A2, A1, A0, 12);

//general UI
byte screen = 0;
volatile bool state = true;

//timers
//state timer
unsigned long stateTimer = 0; 
int stateDelay = 2500;
const int shortStateDelay = 2500;
const int longStateDelay = 10000;
//backlight timer
unsigned long lightTimer = 0; 
int lightDelay = 10000;
const unsigned int longLightDelay = 30000;
bool backlight = true;
//sleep timer
const unsigned int sleepDelay = 60000;

//WDT function
ISR(WDT_vect) {
  if(f_wdt == 0) {
    f_wdt=1;
  }
}

void setup() {
  //lcd setup
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  lcd.begin(16, 2);
  lcd.createChar(0, degree);
  lcd.noCursor();
  lcd.clear();
  lcd.print("Starting up!");
  
  //ch-pd pin
  pinMode(19, OUTPUT);
  digitalWrite(19, HIGH);

  //buttons
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);

  //lights
  for(int i=5; i<10; i++) {pinMode(i, OUTPUT);digitalWrite(i, LOW);}
  RGBoff();

  //start serial coms
  Serial.begin(9600);
  Serial.setTimeout(15000);

  //WDT setup
  //prevent reseting
  MCUSR &= ~(1<<WDRF);
  //enable WDT
  WDTCSR |= (1<<WDCE) | (1<<WDE);
  //set prescaler
  WDTCSR = 1<<WDP0 | 1<<WDP1 | 1<<WDP2;
  //enable interupt
  WDTCSR |= _BV(WDIE);

  //update weather data
  getData();

  //update display with new data
  updateLCD();

  //update LEDs with new data as well
  updateLED();
  
}

void loop() {

  //check for WDT
  if((f_wdt == 1)&&(sleep==true)) {
    
    //clear WDT flag
    f_wdt = 0;

    //increment refresh data counter
    refresh++;

    //check if an update is in order
    if(refresh>=900) {
      refresh=0;
      getData();
    }

    //switch state
    state = !state;
    updateLCD();

    //Re-enter sleep mode
    sleepNow();
    
  }

  //update button
  if(digitalRead(4)==LOW) {
    stateTimer = millis();
    stateDelay = longStateDelay;
    lightTimer = millis();
    lightDelay = longLightDelay;
    backlight = true;
    digitalWrite(13, LOW);
    getData();
  }

  //state button
  if(digitalRead(3)==LOW) {
    stateTimer = millis();
    stateDelay = longStateDelay;
    lightTimer = millis();
    lightDelay = longLightDelay;
    backlight = true;
    digitalWrite(13, LOW);
    state = !state;
    updateLCD();
    updateLED();
    delay(500);
  }

  //next screen button
  if(digitalRead(2)==LOW) {
    stateTimer = millis();
    stateDelay = longStateDelay;
    lightTimer = millis();
    lightDelay = longLightDelay;
    backlight = true;
    digitalWrite(13, LOW);
    screen++;
    if (screen==11) {
      screen = 0;
    }
    updateLCD();
    updateLED();
    delay(500);
  }

  //auto switch state
  if((millis() - stateTimer >= stateDelay)) {
    stateTimer = millis();
    stateDelay = shortStateDelay;
    state = !state;
    updateLCD();
  }

  //auto dim backlight
  if((backlight==true)&&(millis() - lightTimer >= lightDelay)) {
    backlight = false;
    digitalWrite(13, HIGH);
    screen = 0;
    updateLCD();
    updateLED();
  }

  //auto sleep
  if(millis() - lightTimer >= sleepDelay) {
    //turn off all lights
    for(int i=5; i<10; i++) {digitalWrite(i, LOW);}
    RGBoff();

    //sleep
    sleepNow();

  }
  
  delay(50);
  
}

void updateLCD() {

  if (screen == 0) {
    lcd.clear();
    lcd.print("Current Weather:");
    lcd.setCursor(0,1);
    if (state) {
      lcd.print(text[screen]);
    } else {
      lcd.print(temp);
      lcd.write(byte(0));
      lcd.print("C Wind:");
      lcd.print(chill);
      lcd.write(byte(0));
      lcd.print("C");
    }
  } else {
    lcd.clear();
    lcd.print(Day[screen]);
    lcd.print(", ");
    lcd.print(date[screen]);
    lcd.setCursor(0,1);
    if (state) {
      lcd.print(text[screen]);
    } else {
      lcd.print("L:");
      lcd.print(low[screen]);
      lcd.write(byte(0));
      lcd.print("C H:");
      lcd.print(high[screen]);
      lcd.write(byte(0));
      lcd.print("C");
    }
  }
  
}

void updateLED() {

  //clear existing lights
  for(int i=5; i<10; i++) {digitalWrite(i, LOW);}
  RGBoff();

  //find current display code
  int currentCode = code[screen];

  //emergency
  if(currentCode==0||currentCode==1||currentCode==2) {digitalWrite(5, HIGH);digitalWrite(9, HIGH);RGBred();}

  //thunderstorms
  if(currentCode==37||currentCode==38||currentCode==39||currentCode==3||currentCode==4||currentCode==45||currentCode==47) {digitalWrite(5, HIGH);digitalWrite(6, HIGH);digitalWrite(7, HIGH);}

  //rain & snow
  if(currentCode==5||currentCode==6||currentCode==35) {digitalWrite(6, HIGH);digitalWrite(7, HIGH);digitalWrite(8, HIGH);}

  //rain
  if(currentCode==8||currentCode==9||currentCode==10||currentCode==11||currentCode==12||currentCode==40) {digitalWrite(6, HIGH);digitalWrite(7, HIGH);}

  //snow
  if(currentCode==7||currentCode==13||currentCode==14||currentCode==15||currentCode==16||currentCode==17||currentCode==18||currentCode==41||currentCode==42||currentCode==43||currentCode==46) {digitalWrite(8, HIGH);}
  
  //foggy

  //windy

  //cold
  if(currentCode==25) {RGBblue();}

  //hot
  if(currentCode==36) {RGBred();}

  //cloudy
  if(currentCode==26||currentCode==27||currentCode==28) {digitalWrite(7, HIGH);}

  //sunny & cloudy
  if(currentCode==29||currentCode==30||currentCode==44) {digitalWrite(7, HIGH);digitalWrite(9, HIGH);}

  //sunny
  if(currentCode==32||currentCode==34) {digitalWrite(9, HIGH);}  

  //clear

  //error
  if(currentCode==3200) {error();}
  
}

void getData() {
  //lcd message
  lcd.clear();
  lcd.print("Updating!");

  //trigger esp8266 reset
  digitalWrite(19, HIGH);
  delay(250);
  digitalWrite(19, LOW);
  delay(250);
  digitalWrite(19, HIGH);

  //wait for response from esp8266
  while (!Serial.available()) {
    //auto dim backlight
    if((backlight==true)&&(millis() - lightTimer >= lightDelay)) {
      backlight = false;
      digitalWrite(13, HIGH);
   }};

  lcd.setCursor(0,1);
  lcd.print("Data recieved!");
  
  Serial.find("chill=");
  chill = Serial.parseInt();
  
  Serial.find("humidity=");
  humidity = Serial.parseInt();

  Serial.find("sunrise=");
  Serial.readStringUntil('"');
  sunrise = Serial.readStringUntil('"');
  
  Serial.find("sunset=");
  Serial.readStringUntil('"');
  sunset = Serial.readStringUntil('"');

  Serial.find("code=");
  code[0] = Serial.parseInt();

  Serial.find("temp=");
  temp = Serial.parseInt();

  Serial.find("text=");
  Serial.readStringUntil('"');
  text[0] = Serial.readStringUntil('"');

  //store 10-day forecast
  for(int x = 1; x<=10; x++) {
    Serial.find("code=");
    code[x] = Serial.parseInt();
    
    Serial.find("date=");
    Serial.readStringUntil('"');
    date[x] = Serial.readStringUntil('"');

    Serial.find("day=");
    Serial.readStringUntil('"');
    Day[x] = Serial.readStringUntil('"');

    Serial.find("high=");
    high[x] = Serial.parseInt();

    Serial.find("low=");
    low[x] = Serial.parseInt();

    Serial.find("text=");
    Serial.readStringUntil('"');
    text[x] = Serial.readStringUntil('"');
  }

  //dump remaining serial data
  for (int i = 0; i < 10; i++) {
   while (Serial.available() > 0) {
     char k = Serial.read();
     delay(1);
   }
   delay(1);
  }

  lcd.setCursor(0,1);
  lcd.print("Data processed!");

  //restart timers just in case
  stateTimer = millis();
  lightTimer = millis();

}

void sleepNow() {
    //setup interupts
    attachInterrupt(digitalPinToInterrupt(2), detachButton, LOW);
    delay(100);
    attachInterrupt(digitalPinToInterrupt(3), detachButton, LOW);
    delay(100);

    //set sleep flag
    sleep = true;
    
    // Choose our preferred sleep mode:
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
 
    // Set sleep enable (SE) bit:
    sleep_enable();
 
    // Put the device to sleep:
    sleep_mode();
 
    // Upon waking up, sketch continues from this point.
    sleep_disable();

    power_all_enable();
}

void error() {
  lcd.print("ERROR");
  while(true) {
    RGBred();
    delay(250);
    RGBoff();
    delay(250);
  }
}

void RGBred() {
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  digitalWrite(10, HIGH);
  digitalWrite(11, LOW);
}

void RGBblue() {
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  digitalWrite(10, LOW);
  digitalWrite(11, HIGH);
}

void RGBoff() {
  pinMode(10, INPUT);
  pinMode(11, INPUT);
}

void detachButton() {
  detachInterrupt(digitalPinToInterrupt(2));
  detachInterrupt(digitalPinToInterrupt(3));
  sleep = false;
}

