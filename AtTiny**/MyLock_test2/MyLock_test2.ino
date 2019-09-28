#if defined(__AVR_ATtiny85__)
  #include <frequency_counter_attiny.h>
  
  //pins
  #define SERVOPIN 4
  #define LED1PIN 2
  #define LED2PIN 3
  #define LEDOUTPIN 0
  #define LOCKEDBTN 1
#else
  #include <frequency_counter_PCI.h>

  //pins
  #define SERVOPIN 4
  #define LED1PIN 2
  #define LED2PIN 3
  #define LEDOUTPIN 10
  #define LOCKEDBTN 11
#endif


//SET PHONE VALUE HERE
#define UNLOCKVAL 20
bool justOpened = false;
bool justClosed = false;

//sleep libraries
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>

//servo
#include <Servo.h>
Servo servo;

//timing vars
volatile int loops = 0;

//battery vars
bool lowBat = false;
int flashCount = 0;

/*SLEEP SECTION*/

//wake-up routine
ISR(WDT_vect) { 
  wdt_disable();
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
  #if defined(__AVR_ATtiny85__)
    WDTCR |= (1<<WDCE) | (1<<WDE); //Set WD_change enable, set WD enable
    WDTCR = bb; //Set new watchdog timeout value
    WDTCR |= _BV(WDIE); //Set the interrupt enable, this will keep unit from resetting after each int

  #else
    WDTCSR |= (1<<WDCE) | (1<<WDE); //Set WD_change enable, set WD enable
    WDTCSR = bb; //Set new watchdog timeout value
    WDTCSR |= _BV(WDIE); //Set the interrupt enable, this will keep unit from resetting after each int

  #endif
  
}

//sleep routine
void enterSleep(void) {

  //Serial.print("Sleeping...");
  //Serial.flush();

  //noInterrupts();
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);  
  setup_watchdog(6);
  sleep_enable();
  
  //enter sleep
  sleep_mode();
  
  //isr routine

  //wake up
  sleep_disable();
  power_all_enable();
  //interrupts();

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
  
  if (getBatteryVoltage() < 2500) {
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
//get the average of a few detection (foreground) readings
bool takeReading() {
  int freq1 = count_frequency(LED1PIN);
  int freq2 = count_frequency(LED2PIN);
  
  if (freq1 != 0 || freq2 != 0) {
    return true;
  }
  return false;
}




/*LOCK SECTION*/

//open the lock (enable reg and turn servo)
void openLock() {
  //Serial.println("OPENING");

  servo.attach(SERVOPIN);
  digitalWrite(LEDOUTPIN, HIGH);
  delay(250);
  servo.write(165);
  delay(750);
  digitalWrite(LEDOUTPIN, LOW);
  servo.detach();

  justOpened = true;
}

//close the lock (enable reg and turn servo)
void closeLock() {
  //Serial.println("CLOSING");

  servo.attach(SERVOPIN);
  digitalWrite(LEDOUTPIN, HIGH);
  delay(250);
  servo.write(45);
  delay(750);
  digitalWrite(LEDOUTPIN, LOW);
  servo.detach();
  
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
  bool reading = takeReading();
  if (reading && !justOpened && !lockIsOpen()) {
    openLock();
  }
  
  
}


//every 5 mins we will check the battery and get an average reading
void every5mins() {
  checkBattery();
}


void setup() {
  OSCCAL = 0;
  
  //Serial
  //Serial.begin(9600);
  
  //pinModes
  pinMode(LOCKEDBTN, INPUT_PULLUP);
  pinMode(LEDOUTPIN, OUTPUT);
  digitalWrite(LEDOUTPIN, LOW);
  

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
