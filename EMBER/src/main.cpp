#include <Arduino.h>

// NOTE: This project targets an ESP32 (see platformio.ini). Some GPIOs
// (notably 6..11) are connected to the flash/SPI bus and must NOT be used.
// Using those pins can prevent the board from booting, which would also
// explain why you saw no Serial output.

// Choose safe GPIO pins for direction inputs. Change these to match your
// wiring. If your H-bridge also has an ENABLE or PWM pin, make sure it is
// driven HIGH or given PWM as appropriate.
const int in1 = 16; // safe default GPIO
const int in2 = 17; // safe default GPIO

void setup() 
{
  // Start serial and give the monitor a short time to attach
  Serial.begin(115200);
  delay(200);
  Serial.println("Booting...");

  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);

  // Ensure motor is stopped initially
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  Serial.print("Using IN1="); Serial.println(in1);
  Serial.print("Using IN2="); Serial.println(in2);
  Serial.println("Initial stop state set");
}

void loop() 
{
  // Forward
  Serial.println("Forward");
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  delay(1000);

  // Backward
  Serial.println("Backward");
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  delay(1000);

  // Stop
  Serial.println("Stop");
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  delay(1000);
}