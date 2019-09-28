/*
  DigitalReadSerial
 Reads a digital input on pin 2, prints the result to the serial monitor

 This example code is in the public domain.
 */

// digital pin 2 has a pushbutton attached to it. Give it a name:


// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  // make the pushbutton's pin an input:
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  pinMode(4, INPUT);
  pinMode(5, INPUT);
}

// the loop routine runs over and over again forever:
void loop() {
  
  Serial.print(digitalRead(2));
  Serial.print(digitalRead(3));
  Serial.print(digitalRead(4));
  Serial.println(digitalRead(5));
  delay(50);        // delay in between reads for stability
}



