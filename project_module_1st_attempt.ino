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
int readed = 0;
int gameOn = 1;
int direct = 1;
int line = 0;
int dlay = 200;
int x = 7;
int s = 0;

int next_x = 0;
int next_s = 0;

char block[8];
int enable = 0;
void setup() {
  
  Serial.begin(9600);
  Wire.begin(9);
  Wire.onReceive(receiveEvent);
  
  lcd.begin(16, 2);
  lcd.clear();
  
  for(int i = 0; i < 8; i++)
      block[i] = 'z'+144;
  pinMode(OUT_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(IN_PIN, INPUT_PULLUP);
  digitalWrite(OUT_PIN, LOW);
  ledValue = 0;
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonInterrupt, FALLING);
  enable = digitalRead(IN_PIN);

}

void loop() {
  if(enable == 1 || readed == 1){
    if (initial == 1){
       buttonValue = 0;
      if(readed == 1){
        Serial.print("Was here!");
        x = prev_x;
        s = prev_s;
        dlay = 50;
      }
      else
        dlay = 200;      
      initial = 0;
      line = 0;
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
    else if (buttonValue == 1 && gameOn == 1)
    {
      
      buttonValue = 0;
      if(line == 0)
      {
        dlay = 100;
        line = 1;
        if(prev_x != -1 && prev_s != -1){
          if(x != prev_x){
            if(x + (8-s) <= prev_x || prev_x + (8-prev_s) <= x){
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
      else
      {
        gameOn = 0;
        if(x != prev_x){
          if(x + (8-s) <= prev_x || prev_x + (8-prev_s) <= x){
            lcd.setCursor(0, line);
            lcd.print("                ");
            Serial.print("Game Over!");
            lcd.setCursor(0, line);
            lcd.print("Game Over!");
            gameOn = 0;
          }
          else{
            if(x > prev_x)
              s = s + x - prev_x -2;
            else
              s = s + prev_x - x -2;
            next_x = x;
            next_s = s;
            
            Serial.print("Game On!");
            
            sendEvent();
            delay(1000);
            digitalWrite(OUT_PIN, HIGH);
            lcd.setCursor(0, line);
            lcd.print("                ");
            lcd.setCursor(x, line);
            lcd.print(block + s);
          }
        }
      }
    }
  }
}
void buttonInterrupt()
{  
  if(buttonValue == 0)
    buttonValue = 1;
}
void receiveEvent(int bytes) {
  prev_s = Wire.read(); //val_x_s & 15;
  prev_x = Wire.read(); //val_x_s >> 4;
  readed = 1;
}
void sendEvent(){
  int val_x_s_2 = next_x << 4 + next_s & 15;
  Wire.beginTransmission(9);
  Wire.write(next_s);
  Wire.write(next_x);
  Wire.endTransmission(); 
}
