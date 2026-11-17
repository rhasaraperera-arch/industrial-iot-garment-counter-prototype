# industrial-iot-garment-counter-prototype
A standalone Industrial IoT garment-counting prototype powered by an ESP32 microcontroller, featuring a 4-wire I2C LCD operator display and an interrupt-driven infrared optical sensor with robust software debouncing.
# Industrial IoT Garment-Counting Prototype

An edge-computing prototype designed for textile manufacturing lines, utilizing an ESP32 microcontroller, an infrared optical sensor, and an I2C LCD display for real-time piece counting and local telemetry.

---

## 🚀 Project Overview
In industrial garment assembly, accurate real-time throughput tracking is critical for identifying bottlenecks and recording production volume. This project documents the design, assembly, and firmware implementation of a standalone **prototype** that captures fast-moving fabric passes via hardware interrupts and displays verified counts instantly on a local screen.

---

## 🧰 Bill of Materials (BOM)
* **ESP32 Development Board** (Core microcontroller)
* **TCRT5000 IR Sensor Module** (Optical garment detection)
* **16x2 LCD Display with I2C Backpack** (Operator interface)
* **Jumper Wires & Breadboard** (Prototyping interconnects)

---

## 🔌 Wiring & Pinout Guide
Migrating from a parallel 10-wire display layout to an **I2C backpack** streamlined the build down to a clean 4-wire display bus:

### I2C LCD Display
* **GND** -> ESP32 `GND`
* **VCC** -> ESP32 `5V (VIN)`
* **SDA** -> ESP32 `GPIO 21`
* **SCL** -> ESP32 `GPIO 22`

### TCRT5000 IR Sensor
* **VCC** -> ESP32 `3.3V / 5V`
* **GND** -> ESP32 `GND`
* **OUT** -> ESP32 `GPIO 13` (Configured for hardware interrupts)

---

## 💻 Firmware Implementation

```cpp
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Initialize I2C LCD at address 0x3F with a 16x2 layout
LiquidCrystal_I2C lcd(0x3F, 16, 2);

const int irSensorPin = 13;
volatile int passCount = 0;
int lastDisplayedCount = -1; // Prevents unnecessary screen flicker

volatile unsigned long lastInterruptTime = 0;
const unsigned long debounceDelay = 400; // 400ms software debounce lockout

void IRAM_ATTR countPasses() {
  unsigned long currentTime = millis();
  if (currentTime - lastInterruptTime > debounceDelay) {
    passCount++;
    lastInterruptTime = currentTime;
  }
}

void setup() {
  Serial.begin(115200);
  
  // Initialize LCD display and turn on backlight
  lcd.init();                      
  lcd.backlight();
  
  lcd.setCursor(0, 0);
  lcd.print("Garment Tracker");
  lcd.setCursor(0, 1);
  lcd.print("Passes: 0");

  pinMode(irSensorPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(irSensorPin), countPasses, FALLING);
}

void loop() {
  // Refresh display and Serial monitor only when count updates
  if (passCount != lastDisplayedCount) {
    lcd.setCursor(8, 1);
    lcd.print("        "); // Clear previous digits
    lcd.setCursor(8, 1);
    lcd.print(passCount);
    
    Serial.print("Verified Pass Count: ");
    Serial.println(passCount);

⚙️ Engineering Iterations & Challenges
1. Display Optimization: Solved complex parallel wiring bottlenecks by integrating an I2C adapter backpack, reducing connections and resolving address configurations (0x3F).
2. Signal Filtering: Implemented a strict millis()-based software debounce lockout (400ms) inside an ISR (IRAM_ATTR) to eliminate false triggers caused by light flutter or mechanical jitter.
🏭 Scaling from Prototype to Production
While this prototype successfully validates interrupt-driven logic and local UI feedback on a workbench, deploying optical counting on a live factory floor requires addressing industrial constraints:
Bullet Dust & Lint Resistance: Airborne textile dust can obscure exposed optical lenses, necessitating IP67-rated sealed sensor barrels.
Bullet Ambient Light Rejection: High-frequency factory lighting can saturate unmodulated photo-transistors, requiring modulated infrared pulsed beams.
Bullet Electrical Noise: Long cabling runs require 24V industrial signaling rather than direct 3.3V microcontroller logic inputs.


    lastDisplayedCount = passCount;
  }
}
