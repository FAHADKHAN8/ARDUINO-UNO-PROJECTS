#include <Servo.h>  // Include the servo library
#include <Wire.h>   // Include the wire library
#include <LiquidCrystal_I2C.h>     // Include the LiquidCrystal library

// Initialize LCD with I2C address 0x27, 20x4 characters
LiquidCrystal_I2C lcd(0x27, 20, 4);

Servo myservo;  // Create a Servo object

// Define IR sensor pins for each parking slot and entry/exit
#define ir_enter 2
#define ir_back  4
#define ir_car1 5
#define ir_car2 6
#define ir_car3 7
#define ir_car4 8

// Variables to store sensor readings for each parking slot
int S1 = 0, S2 = 0, S3 = 0, S4 = 0;
int flag1 = 0, flag2 = 0;  // Flags to track entry and exit actions
int slot = 4;  // Total number of parking slots

void setup() {
  Serial.begin(9600);

  // Set IR sensor pins as inputs
  pinMode(ir_car1, INPUT);
  pinMode(ir_car2, INPUT);
  pinMode(ir_car3, INPUT);
  pinMode(ir_car4, INPUT);
  
  // Set entry and exit IR pins as inputs
  pinMode(ir_enter, INPUT);
  pinMode(ir_back, INPUT);

  // Attach the servo to digital pin 3 and initialize its position
  myservo.attach(3);
  myservo.write(10);

  // Initialize and configure the LCD
  lcd.init();
  lcd.backlight();
  lcd.begin(20, 4);
  lcd.setCursor(0, 1);
  lcd.print("    Car  parking  ");
  lcd.setCursor(0, 2);
  lcd.print("       System     ");
  delay(2000);
  lcd.clear();

  // Read initial sensor states
  Read_Sensor();

  // Calculate available parking slots
  int total = S1 + S2 + S3 + S4;
  slot = slot - total;
}

void loop() {
  // Read sensor states periodically
  Read_Sensor();

  // Display available slots and status for each slot
  lcd.setCursor(0, 0);
  lcd.print("   Available Slots: ");
  lcd.print(slot);
  lcd.print("    ");

  lcd.setCursor(0, 1);
  lcd.print("S1: ");
  lcd.print(S1 == 1 ? "FULL" : "EMPTY");

  lcd.setCursor(10, 1);
  lcd.print("S2: ");
  lcd.print(S2 == 1 ? "FULL" : "EMPTY");

  lcd.setCursor(0, 2);
  lcd.print("S3: ");
  lcd.print(S3 == 1 ? "FULL" : "EMPTY");

  lcd.setCursor(10, 2);
  lcd.print("S4: ");
  lcd.print(S4 == 1 ? "FULL" : "EMPTY");

  // Check for entry signal and available slots
  if (digitalRead(ir_enter) == 0 && flag1 == 0) {
    if (slot > 0) {
      flag1 = 1;
      if (flag2 == 0) {
        // Open the entry gate
        myservo.write(100);  // Assuming 100 degrees corresponds to open
        slot = slot - 1;
      }
    } else {
      lcd.setCursor(0, 0);
      lcd.print(" Parking Full ");  
      delay(1500);
    }   
  }

  // Check for exit signal
  if (digitalRead(ir_back) == 0 && flag2 == 0) {
    flag2 = 1;
    if (flag1 == 0) {
      // Open the exit gate
      myservo.write(100);  // Assuming 100 degrees corresponds to open
      slot = slot + 1;
    }
  }

  // Reset flags and close the gates after a delay
  if (flag1 == 1 && flag2 == 1) {
    delay(1000);
    myservo.write(10);  // Assuming 10 degrees corresponds to closed
    flag1 = 0, flag2 = 0;
  }

  delay(1);
}

void Read_Sensor() {
  // Read sensor states and update slot status
  S1 = digitalRead(ir_car1);
  S2 = digitalRead(ir_car2);
  S3 = digitalRead(ir_car3);
  S4 = digitalRead(ir_car4);
}
