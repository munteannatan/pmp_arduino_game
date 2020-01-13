#include <Wire.h>
#include <LiquidCrystal.h>

#define BUTTON_PIN 19
#define OUT_PIN 24
#define IN_PIN 25
#define LED_PIN 13

LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

int ledValue;
volatile int buttonValue;

int prev_x = -1;
int prev_s = -1;

int initial = 1;
int gameOn = 1;
int direct = 1;
int line = 0;
int dlay = 200;
int x = 7;
int s = 0;

int next_x = 0;
int next_s = 0;

char block[8];

void setup() {
  Wire.begin(9);
  Wire.onReceive(receiveEvent);
  
  lcd.begin(16, 2);
  lcd.clear();
  
  for(int i = 0; i < 8; i++)
      block[i] = 'z'+145;
  pinMode(OUT_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(IN_PIN, INPUT_PULLUP);
  digitalWrite(OUT_PIN, LOW);
  ledValue = 0;
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonInterrupt, FALLING);

}

void loop() {
  int enable = digitalRead(IN_PIN);
  if(enable == 1){
    if (initial == 1){
      if(prev_x != -1 && prev_s != -1){
        x = prev_x;
        y = prev_y;
        dlay = 50;
      }
      else
        dlay = 200;      
      initial = 0;
    }
    if(buttonValue == 0 && gameOn == 1){
      lcd.setCursor(x, line);
      lcd.print("        ");
      x += direct;
      if(x + 8 - s== 16){
        direct = -1;
      }
      if(x == 0){
        direct = 1;
      }
      lcd.setCursor(x, line);
      lcd.print(block + s);
      delay(dlay);
    }
    else if (gameOn == 1)
    {
      if(line == 0)
      {
        dlay = 100;
        line = 1;
        if(prev_x != -1 && prev_s != -1){
          if(x != prev_x){
            if(x + (8-s) < prev_x || prev_x + (8-prev_s) < x){
              lcd.setCursor(x, line);
              lcd.print("Game Over!");
              gameOn = 0;
            }
            else{
              if(x > prev_x)
                s = s + x - prev_x;
              else
                s = s + prev_x - x;
            }
            prev_x = x;
            prev_s = s;
          }
        }
      }
      if(line == 1)
      {
        gameOn = 0;
        if(prev_x != -1 && prev_s != -1){
          if(x != prev_x){
            if(x + (8-s) < prev_x || prev_x + (8-prev_s) < x){
              lcd.setCursor(x, line);
              lcd.print("Game Over!");
              gameOn = 0;
            }
            else{
              if(x > prev_x)
                s = s + x - prev_x;
              else
                s = s + prev_x - x;
            }
            next_x = x;
            next_s = s;
          }
        }
        sendEvent();
      }
      buttonValue = 0;
    }
  }
}
void buttonInterrupt()
{  
  if(buttonValue == 0)
    buttonValue = 1;
}
void receiveEvent(int bytes) {
  int val_x_s = Wire.read();
  prev_s = val_x_s & 15;
  prev_x = val_x_s >> 4;
}
void sendEvent(){
  int val_x_s = next_x << 4 + next_s & 15;
  Wire.beginTransmission(9);
  Wire.write(val_x_s);
  Wire.endTransmission(); 
  
  digitalWrite(OUT_PIN, HIGH);
}
