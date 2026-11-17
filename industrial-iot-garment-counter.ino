#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Set LCD address 
LiquidCrystal_I2C lcd(0x3F, 16, 2);

const int irSensorPin = 13;
volatile int passCount = 0;
int lastDisplayedCount = -1; // Prevent screen flickering

volatile unsigned long lastInterruptTime = 0;
const unsigned long debounceDelay = 400; 

void IRAM_ATTR countPasses() {
  unsigned long currentTime = millis();
  if (currentTime - lastInterruptTime > debounceDelay) {
    passCount++;
    lastInterruptTime = currentTime;
  }
}

void setup() {
  Serial.begin(115200);
  
  // Initialize the LCD
  lcd.init();                      
  lcd.backlight();
  
  lcd.setCursor(0, 0);
  lcd.print("FLOOR Line Cnt");
  lcd.setCursor(0, 1);
  lcd.print("Passes: 0");

  pinMode(irSensorPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(irSensorPin), countPasses, FALLING);
}

void loop() {
  
  if (passCount != lastDisplayedCount) {
    lcd.setCursor(8, 1);
    lcd.print("        "); 
    lcd.setCursor(8, 1);
    lcd.print(passCount);
    
    Serial.print("Accurate Pass Count: ");
    Serial.println(passCount);
    
    lastDisplayedCount = passCount;
  }
}