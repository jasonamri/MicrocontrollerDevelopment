void setup() {
  // put your setup code here, to run once:
 pinMode(4, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(1, OUTPUT);
  pinMode(0, OUTPUT);
  digitalWrite(4, LOW);
  digitalWrite(3, HIGH);
  digitalWrite(1, LOW);
  digitalWrite(0, LOW);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
      Serial.println("system on");
      digitalWrite(4, HIGH);
      
      Serial.println("amp on");
      digitalWrite(3, LOW);
      delay(100);
      Serial.println("amp on good");
      digitalWrite(3, HIGH);
      
      Serial.println("get ready");
      delay(12000);
      Serial.println("3");
      delay(1000);
      Serial.println("2");
      delay(1000);
      Serial.println("1");
      delay(1000);
      
      Serial.println("volume up");
      digitalWrite(0, HIGH);
      digitalWrite(1, HIGH);
      delay(5000);
      Serial.println("amp volume good");
      digitalWrite(1, LOW);
      delay(2500);
      Serial.println("mp3 volume good");
      digitalWrite(0, LOW);
     
      
      
      
      Serial.println("starting shutdown timer");
      delay(30000);
      Serial.println("system off");
   
      digitalWrite(4, LOW);
      
      Serial.println("starting boot timer");
      delay(5000);
   
}
