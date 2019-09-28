//SET PHONE VALUE HERE
#define UNLOCKVAL 54000
bool justOpened = false;
bool justClosed = false;

//sleep libraries
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>

//servo
#include <Servo.h>
Servo servo;

//pins
#define SERVOPIN 4
#define LED1PIN A1
#define LED2PIN A3
#define LEDOUTPIN 0
#define LOCKEDBTN 1

//readings average 
#define BACKGROUND 20
#define FOREGROUND 1
#define TIMEOUT 500
int average = 900;

//timing vars
volatile int watching = 1;
volatile bool watchdogged = 0;
volatile int loops = 0;


//battery vars
bool lowBat = false;
int flashCount = 0;




/*SLEEP SECTION*/

//wake-up routine
ISR(WDT_vect) {
  if (watching==1) {
    watching = 0;
    watchdogged = 1;
  }
}

//Sets the watchdog timer to wake us up, but not reset
//0=16ms, 1=32ms, 2=64ms, 3=128ms, 4=250ms, 5=500ms
//6=1sec, 7=2sec, 8=4sec, 9=8sec
void setup_watchdog(int timerPrescaler) {

  if (timerPrescaler > 9 ) timerPrescaler = 9; //Limit incoming amount to legal settings

  byte bb = timerPrescaler & 7; 
  if (timerPrescaler > 7) bb |= (1<<5); //Set the special 5th bit if necessary

  //This order of commands is important and cannot be combined
  MCUSR &= ~(1<<WDRF); //Clear the watch dog reset
  WDTCR |= (1<<WDCE) | (1<<WDE); //Set WD_change enable, set WD enable
  WDTCR = bb; //Set new watchdog timeout value
  WDTCR |= _BV(WDIE); //Set the interrupt enable, this will keep unit from resetting after each int
}

//sleep routine
void enterSleep(void) {

  //Serial.print("Sleeping...");
  //Serial.flush();
  
  watching = 1;
  
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);  
  setup_watchdog(5);
  sleep_enable();
  
  //enter sleep
  sleep_mode();
  
  //isr routine

  //wake up
  sleep_disable();
  power_all_enable();

  //Serial.println("Woke up!");

}




/*BATTERY SECTION*/

//returns VCC (battery) voltage in mV
long getBatteryVoltage() {
  //reads internal 1V1 reference against VCC
  #if defined(__AVR_ATtiny84__) || defined(__AVR_ATtiny44__)
    ADMUX = _BV(MUX5) | _BV(MUX0); // For ATtiny84
  #elif defined(__AVR_ATtiny85__) || defined(__AVR_ATtiny45__)
    ADMUX = _BV(MUX3) | _BV(MUX2); // For ATtiny85/45
  #elif defined(__AVR_ATmega1284P__)
    ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);  // For ATmega1284
  #else
    ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);  // For ATmega328
  #endif
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Convert
  while (bit_is_set(ADCSRA, ADSC));
  uint8_t low = ADCL;
  unsigned int val = (ADCH << 8) | low;
  //discard previous result
  ADCSRA |= _BV(ADSC); // Convert
  while (bit_is_set(ADCSRA, ADSC));
  low = ADCL;
  val = (ADCH << 8) | low;
  
  return ((long)1024 * 1100) / val;  
  //return 2499;
}

//if battery is less than 2.5V, set a flag
void checkBattery() {

  //Serial.print("Battery Voltage: ");
  //Serial.println(getBatteryVoltage());
  
  if (getBatteryVoltage() < 2600) {
    lowBat = true;
  } else {
    lowBat = false;
  }
  
}

//if the battery is low, we'll blink the LED
void blinkLED() {
  //Serial.println("Blinking LED");
  
  flashCount++;

  if (flashCount == 4) {
    digitalWrite(LEDOUTPIN, HIGH);
  } else if (flashCount == 5) {
    digitalWrite(LEDOUTPIN, LOW);
    flashCount = 0;
  }
  
}




/*READINGS SECTION*/

//get the average of both LEDs
int sharedReading() {
  return ((analogRead(LED1PIN) + analogRead(LED2PIN))/2.0);
}

//check if timed reading is within threshold
bool withinTimeThreshold(unsigned long reading, unsigned long target) {
  return (reading < target+2000 && reading > target-2000);
}

//check if value reading is within threshold
bool withinValueThreshold(unsigned long reading, unsigned long target) {
  return (reading < target+100 && reading > target-100);
}

//get the average of several background readings
void getBackgroundAverage() {

  long total = 0;
  for (int i = 0; i < BACKGROUND; i++) {
    total += sharedReading();
  }
  average = ((long)total / BACKGROUND);

  //Serial.print("Background average: ");
  //Serial.println(average);
}

//get the average of a few detection (foreground) readings
unsigned long takeReading() {
  
  unsigned long timer = micros();
  long buff = 0;
  bool on = 0;

  while (buff <= 0 && micros()-timer < (long)TIMEOUT * 1000) {

    if(withinValueThreshold(sharedReading(), average)) {
      //Serial.println("off");
      //mark as off
      on = 0;
    } else {
      //on a falling edge:
      if(on == 0) {
        on = 1;
  
        //start timer or report result (on second loop)
        if (buff == 0) {
          timer = micros();
          buff = -1;
        } else {
          buff = (micros()-timer);
        }          
      }
    } 
  }

  //Serial.print("Reading: ");
  //Serial.println(buff);
  
  return (buff);
}




/*LOCK SECTION*/

//open the lock (enable reg and turn servo)
void openLock() {
  //Serial.println("OPENING");

  digitalWrite(LEDOUTPIN, HIGH);
  delay(50);
  servo.write(135);
  delay(500);
  digitalWrite(LEDOUTPIN, LOW);

  justOpened = true;
}

//close the lock (enable reg and turn servo)
void closeLock() {
  //Serial.println("CLOSING");

  digitalWrite(LEDOUTPIN, HIGH);
  delay(50);
  servo.write(45);
  delay(500);
  digitalWrite(LEDOUTPIN, LOW);
  
  justClosed = true;
}

//check if lock is closed
bool lockIsOpen() {
  return digitalRead(LOCKEDBTN) == HIGH;
}




/*ROUTINE SECTION*/

//routine to run every sec
void every1sec() {
  //blink red led if bat is low
  if (lowBat) { blinkLED(); }

  //check button
  if (lockIsOpen()) {
    justClosed = false;
    justOpened = false;
  }

  //close the lock
  if (!lockIsOpen() && !justClosed) {
    closeLock();
  }

  //open the lock
  for (int i = 0; i < FOREGROUND; i++) {
    unsigned long reading = takeReading();
    if (withinTimeThreshold(reading, UNLOCKVAL) && reading != 0 && !justOpened && !lockIsOpen()) {
      openLock();
    }
  }
  
}


//every 5 mins we will check the battery and get an average reading
void every5mins() {
  checkBattery();
  getBackgroundAverage();
}




void setup() {
  //Serial
  //Serial.begin(115200);
  
  //pinModes
  pinMode(LED1PIN, INPUT);
  pinMode(LED2PIN, INPUT);
  pinMode(LOCKEDBTN, INPUT_PULLUP);
  pinMode(LEDOUTPIN, OUTPUT);
  digitalWrite(LEDOUTPIN, LOW);
  servo.attach(SERVOPIN);

  //get average reading and check low bat
  every5mins();
  
}




void loop() {

  enterSleep();

  //on wake-up:
  loops++;

  //every 1 sec
  every1sec();


  //after 5 mins
  if (loops == 600) {
  //if (loops == 20) {
    loops = 0;
    every5mins();
  }

}
