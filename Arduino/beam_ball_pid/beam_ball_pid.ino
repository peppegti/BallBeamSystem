#include <Servo.h>
#include <PID_v1.h>
#include "Adafruit_VL53L1X.h"


// Definizione pin e oggetti
#define IRQ_PIN 2
#define XSHUT_PIN 3


Adafruit_VL53L1X vl53 = Adafruit_VL53L1X(XSHUT_PIN, IRQ_PIN);


const int servoPin = 9;
const int setposition = A0;  // Collega il potenziometro al pin analogico A0
const int set_p = A1;  // Collega il potenziometro al pin analogico A1
const int set_i = A2;  // Collega il potenziometro al pin analogico A2
const int set_d = A3;  // Collega il potenziometro al pin analogico A3

const float tolerance = 2.0;  // Tolleranza di 1 cm

// tuning PID here
float Kp = 0.5; // Proportional Gain
float Ki = 0.0; // Integral Gain
float Kd = 0.2; // Derivative Gain
double Setpoint, Input, Output, ServoOutput;

PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT); // Initialize PID object, which is in the class PID.
Servo myServo; // Initialize Servo.
uint32_t last_read = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);
  pinMode(PIN_START, INPUT);

  Serial.println(F("Adafruit VL53L1X sensor demo"));

  Wire.begin();
  if (!vl53.begin(0x29, &Wire)) {
    Serial.print(F("Error on init of VL sensor: "));
    Serial.println(vl53.vl_status);
    while (1) delay(10);
  }
  Serial.println(F("VL53L1X sensor OK!"));

  if (!vl53.startRanging()) {
    Serial.print(F("Couldn't start ranging: "));
    Serial.println(vl53.vl_status);
    while (1) delay(10);
  }
  Serial.println(F("Ranging started"));

  vl53.setTimingBudget(50);
  Serial.print(F("Timing budget (ms): "));
  Serial.println(vl53.getTimingBudget());

  myServo.attach(servoPin); // Attach Servo



  Input = readPosition(); // Sets the initial input to the PID algorithm
  myPID.SetMode(AUTOMATIC); // Set PID object myPID to AUTOMATIC
  myPID.SetOutputLimits(-20, 20); // Set Output limits
  myPID.SetSampleTime(20);
}

void loop() {
    last_read = millis();
    int potValue = analogRead(setposition); // Legge il valore dal potenziometro
    Setpoint = map(potValue, 0, 1023, 0, 66); // Mappa il valore letto da 0 a 1023 a 0 a 66
    Kp = map(analogRead(set_p), 0, 1023, 0, 1000) * 0.01;
    Ki = map(analogRead(set_i), 0, 1023, 0, 500) * 0.0001;
    Kd = map(analogRead(set_d), 0, 1023, 0, 500) * 0.01;

    myPID.SetTunings(Kp, Ki, Kd);
    Input = readPosition();
    myPID.Compute(); // Computes Output in range of -60 to 60 degrees
    ServoOutput = 82 - Output; // 82 degrees is the horizontal, can change depending on the current angle of the motor
    myServo.write(ServoOutput); // Writes value of Output to servo


    // Stampa i valori su Serial per il plotter
    Serial.print("Timestamp:");
    Serial.print(millis());
    Serial.print(",");
    Serial.print("Distance:");
    Serial.print(Input);
    Serial.print(",");
    Serial.print("output:");
    Serial.print(Output);
    Serial.print(",");
    Serial.print("Kp:");
    Serial.print(Kp);
    Serial.print(",");
    Serial.print("Ki:");
    Serial.print(Ki, 4);
    Serial.print(",");
    Serial.print("Kd:");
    Serial.print(Kd, 4);
    Serial.print(",");
    Serial.print("Setpoint:");
    Serial.println(Setpoint);

}

float readPosition() {
  int16_t distance = -1;
  while (distance == -1) {
    if (vl53.dataReady()) {
      distance = vl53.distance();
      if (distance == -1) {
        Serial.print(F("Couldn't get distance: "));
        Serial.println(vl53.vl_status);
      } else {
        //Serial.print(F("Raw Distance: "));
        //Serial.println(distance);
      }
      vl53.clearInterrupt();
    }
    //delay(10); // Aggiungere un piccolo ritardo per evitare un loop troppo veloce
  }
  return distance / 10.0; // Convert mm to cm
}
