
/********************************************************
** Download from:                                      **
** http://www.alhin.de/arduino/index.php?n=44          **
********************************************************/

#include <AH_EasyDriver.h>

//AH_EasyDriver(int RES, int DIR, int STEP, int MS1, int MS2, int SLP, int ENABLE, int RST);
//AH_EasyDriver stepper(200,2,3,4,5,6,7,8);   // init with all functions

//AH_EasyDriver(int RES, int DIR, int STEP, int MS1, int MS2, int SLP);
AH_EasyDriver stepper(96,11,12,10,9,8);    // init w/o "enable" and "reset" functions

//AH_EasyDriver stepper(96,11,12);    // init w/o "enable" and "reset" functions


void setup() {

  //stepper.setMicrostepping(3);            // 0 -> Full Step                                
                                          // 1 -> 1/2 microstepping
                                          // 2 -> 1/4 microstepping
                                          // 3 -> 1/8 microstepping
  stepper.setSpeedRPM(100);               // set speed in RPM, rotations per minute
//  stepper.setSpeedHz(100);              // set speed in Hz
//stepper.sleepOFF(); 
}

void loop() {

 
  stepper.sleepON(); 
  delay(10000);



}


