String str;

void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:

if (Serial.available()>0) {
 
 str = Serial.readStringUntil(' HTTP');
 
 int spacePosition = str.indexOf('=');
 if (str.charAt(spacePosition + 1) == '5') {
    Serial.println("You might have found the Benjamins.");
 }
  
}

}
