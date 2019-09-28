
int mode = 0;
int magnet = 1;
unsigned long preTime = 0;
int raw;

void setup() 
{
  pinMode(A1, INPUT);
  pinMode(4, OUTPUT);
}

void loop() 
{
    
  
    raw = analogRead(1);   // Range : 0..1024
    
     if (raw > 530 || raw < 450) {
    magnet = 1;
    } else {          
    magnet = 0;
    };
    

    if (mode == 0) {
      delay(10);  
      if (magnet == 1) {
        unsigned long curTime = millis();
        if (curTime - preTime >= 625) {
          mode = 1;
        } else {
          mode = 2;
        };
      };
    };
    
    if (mode == 1) {     
      if (magnet == 0) {
        mode = 0;
        preTime = millis();
      };
    };
    
    if (mode == 2) {
      delay(10);  
      if (magnet == 0) {
        mode = 3;
      };
    };
    
    if (mode == 3) { 
      delay(10);        
      if (magnet == 1) {
        mode = 1;
      };
    };
    
    
    if (mode == 1) {
      digitalWrite(4, HIGH);
    } else {
      digitalWrite(4, LOW);
    };
    
    
    
    
    delay(10);
}
