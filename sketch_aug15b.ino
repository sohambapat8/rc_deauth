#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CE_PIN 9
#define CSN_PIN 53

const uint64_t pipe = 0xE8E8F0F0E1LL;   

#define ENA_m1 7          // Enable/speed motor Front Right 
#define ENB_m1 6          // Enable/speed motor Back Right
#define ENA_m2 5          // Enable/speed motor Front Left
#define ENB_m2 4          // Enable/speed motor Back Left

#define IN_11  39          // L298N #1 in 1 motor Front Right
#define IN_12  38          // L298N #1 in 2 motor Front Right
#define IN_13  41          // L298N #1 in 3 motor Back Right
#define IN_14  40          // L298N #1 in 4 motor Back Right

#define IN_21  33         // L298N #2 in 1 motor Front Left
#define IN_22  32         // L298N #2 in 2 motor Front Left
#define IN_23  35         // L298N #2 in 3 motor Back Left
#define IN_24  34         // L298N #2 in 4 motor Back Left

RF24 radio(CE_PIN, CSN_PIN); // Create a Radio

int joystick[6];  // 6 element array holding Joystick readings
int speedRight = 0;
int speedLeft = 0;
int  xAxis, yAxis;

int buttonUp;
int buttonRight;
int buttonDown;
int buttonLeft;

void setup()
{
  pinMode(ENA_m1, OUTPUT);
  pinMode(ENB_m1, OUTPUT);
  pinMode(ENA_m2, OUTPUT);
  pinMode(ENB_m2, OUTPUT);
  
  pinMode(IN_11, OUTPUT);
  pinMode(IN_12, OUTPUT);
  pinMode(IN_13, OUTPUT);
  pinMode(IN_14, OUTPUT);
    
  pinMode(IN_21, OUTPUT);
  pinMode(IN_22, OUTPUT);
  pinMode(IN_23, OUTPUT);
  pinMode(IN_24, OUTPUT);
  
  radio.begin();
  radio.openReadingPipe(1,pipe);
  radio.startListening();
}

void loop()
{
  if ( radio.available() )
  {
      radio.read( joystick, sizeof(joystick) );
      xAxis = joystick[0];
      yAxis = joystick[1];
      
     
      int buttonUp    = joystick[2];
      int buttonRight = joystick[3];
      int buttonDown  = joystick[4];
      int buttonLeft  = joystick[5];
      
    if (yAxis < 470) {
    
    digitalWrite(IN_11, LOW);
    digitalWrite(IN_12, HIGH);
    digitalWrite(IN_13, HIGH);
    digitalWrite(IN_14, LOW);
    
    digitalWrite(IN_21, HIGH);
    digitalWrite(IN_22, LOW);
    digitalWrite(IN_23, LOW);
    digitalWrite(IN_24, HIGH);
    
    speedRight  = map(yAxis, 470, 0, 0, 255);
    speedLeft   = map(yAxis, 470, 0, 0, 255);
  }

else if (yAxis > 550) {
  
    digitalWrite(IN_11, HIGH);
    digitalWrite(IN_12, LOW);
    digitalWrite(IN_13, LOW);
    digitalWrite(IN_14, HIGH);
    
    digitalWrite(IN_21, LOW);
    digitalWrite(IN_22, HIGH);
    digitalWrite(IN_23, HIGH);
    digitalWrite(IN_24, LOW);
 
    speedRight  = map(yAxis, 550, 1023, 0, 255);
    speedLeft   = map(yAxis, 550, 1023, 0, 255);
  }
  else {
    speedRight  = 0;
    speedLeft   = 0;
  }


  if (xAxis < 470) {
    int xMapped = map(xAxis, 470, 0, 0, 255);
    speedLeft = speedLeft - xMapped;
    speedRight = speedRight + xMapped;
    // Confine the range from 0 to 255
    if (speedLeft < 0) {
      speedLeft = 0;
    }
    if (speedRight > 255) {
      speedRight = 255;
    }
  }
  if (xAxis > 550) {
    // Convert the increasing X-axis readings from 550 to 1023 into 0 to 255 value
    int xMapped = map(xAxis, 550, 1023, 0, 255);
    // Move right - decrease right motor speed, increase left motor speed
    speedLeft = speedLeft + xMapped;
    speedRight = speedRight - xMapped;
    // Confine the range from 0 to 255
    if (speedLeft > 255) {
      speedLeft = 255;
    }
    if (speedRight < 0) {
      speedRight = 0;
    }
  }
  // Prevent buzzing at low speeds (Adjust according to your motors. My motors couldn't start moving if PWM value was below value of 70)
  if (speedLeft < 70) {
    speedLeft = 0;
  }
  if (speedRight < 70) {
    speedRight = 0;
  }

  analogWrite(ENA_m1, speedRight); // Send PWM signal to motor A
  analogWrite(ENB_m1, speedRight);
  analogWrite(ENA_m2, speedLeft); // Send PWM signal to motor B
  analogWrite(ENB_m2, speedLeft);
  }
      
}
