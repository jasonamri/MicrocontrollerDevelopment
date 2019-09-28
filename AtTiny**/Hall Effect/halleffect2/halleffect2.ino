
int mode = 0;
int magnet = 1;
unsigned long preTime = 0;
int raw;
int state = 0;

void setup() 
{
  pinMode(A1, INPUT);
  pinMode(4, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(1, OUTPUT);
  pinMode(0, OUTPUT);
  digitalWrite(4, LOW);
  digitalWrite(3, HIGH);
  digitalWrite(1, LOW);
  digitalWrite(0, LOW);
  
}

void loop() 
{
    
  
    raw = analogRead(1)/20;   // Range : 0..1024
    
     if (raw > 22 && raw < 30) {
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

      
       if (state == 0) {
      
     
      digitalWrite(3, LOW);
      delay(12);
      digitalWrite(3, HIGH);
   
      digitalWrite(0, HIGH);
      digitalWrite(1, HIGH);
      
      delay(1000);
    
      digitalWrite(1, LOW);
      digitalWrite(0, LOW);
      
      state = 1; }

      delay(10);
      
    } else {
      digitalWrite(4, LOW);
      state = 0;
    };
    
    
    
    
    delay(10);
}
