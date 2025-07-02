#include <Stepper.h>

// Parameters that must be entered
int measurementsPerTurn = 0;  // Divisor of 2048
int linearmeasurements = 0;
int distanceEnterDiameters = 0;  // in micrometers
int timeForMeasurement = 0;

// Standard parameters
const int stepsPerSmallRotation = 2048;
bool forward = true;
int step = 0;
Stepper linearStepper(stepsPerSmallRotation, 8, 9, 10, 11);
Stepper rotationStepper(stepsPerSmallRotation, 4, 5, 6, 7);
const double ratioOfGears = 6.03;
const int stepsForAmm = 10;

// Calculations rotation
const int stepsPerBigRotation = round(ratioOfGears * (double)stepsPerSmallRotation);
const int stepsBetweenTwoMeasuringPointsRotation = stepsPerBigRotation / measurementsPerTurn;

// Calculations vertical
const int verticalSteps = distanceEnterDiameters * stepsForAmm;

void setup() {
  Serial.begin(9600);
}

void loop() {
  switch (step) {
    case 0:
      set();  //start calibration
    case 1:
      adjustementRotation();  //Turn to find the starting position
    case 2:
      setLinear();
    case 3:
      adjustementLinear();
    case 4:
      start();  //Pause before starting the measurement
    case 5:
      measurement();  //Function that performs the measurement
  }
}

void set() {
  int input;  // Declare a variable to store the input
  int fillingData;
  // Continue looping until 'step' equals 1
  while (step != 1) {
    input = Serial.read();  // Read input from the serial port
    switch (fillingData) {
      case 0:
        measurementsPerTurn = input;
        if (measurementsPerTurn > 0) {
          fillingData = 1;
        }
      case 1:
        linearmeasurements = input;
        if (linearmeasurements > 0) {
          fillingData = 2;
        }
      case 2:
        distanceEnterDiameters = input;
        if (distanceEnterDiameters > 0) {
          fillingData = 3;
        }
      case 3:
        timeForMeasurement = input;
        if (timeForMeasurement > 0) {
          fillingData = 4;
        }
      case 4:
        if (input == '1') {
          step = 1;
        }
    }
  }
}

void adjustementRotation() {
  int inputI;          // Declare a variable to store the input
  int stepsMoved = 0;  // Number of steps already taken

  // Continue looping until 'step' equals 2
  while (step != 2) {
    rotationStepper.setSpeed(2);  // Set the speed of the stepper motor

    // Check if the number of steps moved is less than the steps per small rotation
    if (stepsMoved < stepsPerSmallRotation) {
      rotationStepper.step(1);  // Move the motor one step
      stepsMoved++;
    }

    // Check if there is any data available on the serial port
    if (Serial.available() > 0) {
      inputI = Serial.read();  // Read input from the serial port

      // Check if the input is '2'
      if (inputI == '2') {
        stoppStepper(4, 5, 6, 7);  // Stop the stepper motor
        step = 2;                  // Set 'step' to 2
      }
    }
  }
}

void setLinear() {
  int input;  // Declare a variable to store the input

  // Continue looping until 'step' equals 1
  while (step != 3) {
    input = Serial.read();  // Read input from the serial port

    // Check if the input is '1'
    if (input == '3') {
      step = 3;  // Set 'step' to 1
    }
  }
}

void adjustementLinear() {
  int input;  // Declare a variable to store the input

  // Continue looping until 'step' equals 2
  while (step != 4) {
    linearStepper.setSpeed(2);  // Set the speed of the stepper motor
    linearStepper.step(1);      // Move the motor one step
    // Check if there is any data available on the serial port
    if (Serial.available() > 0) {
      input = Serial.read();  // Read input from the serial port

      // Check if the input is '2'
      if (input == '4') {
        stoppStepper(8, 9, 10, 11);  // Stop the stepper motor
        step = 4;                    // Set 'step' to 2
      }
    }
  }
}

void start() {
  int input;  // Declare a variable to store the input

  // Continue looping until 'step' equals 3
  while (step != 5) {
    // Check if there is any data available on the serial port
    if (Serial.available() > 0) {
      input = Serial.read();  // Read input from the serial port

      // Check if the input is '3'
      if (input == '5') {
        step = 5;  // Set 'step' to 3
      }
    }
  }
}


void measurement() {
  rotationStepper.setSpeed(10);  // Stepper speed for the measurement
  linearStepper.setSpeed(10);

  // Outer loop for vertical movement
  for (int c = 0; c <= linearmeasurements; c++) {
    // Inner loop for the rotation
    for (int i = 1; i < measurementsPerTurn + 1; i++) {
      delay(500);         // Wait for 500 milliseconds
      serialprint(i, c);  // Print measurement data to the serial monitor
      delay(500);         // Wait for 500 milliseconds
      if (forward) {
        rotationStepper.step(stepsBetweenTwoMeasuringPointsRotation);  // Move the stepper motor
      } else {
        rotationStepper.step(-stepsBetweenTwoMeasuringPointsRotation);  // Move the stepper motor
      }
      stoppStepper(8, 9, 10, 11);  // Stop the stepper motor
    }
    serialprint(measurementsPerTurn, c);  // Print measurement data to the serial monitor
    forward = !forward;                   // Toggle the direction of movement
    verticalMovement();
  }

  Serial.println("End");  // Print "End" to the serial monitor

  // Infinite loop to stop the program
  while (true) {
    stoppStepper(8, 9, 10, 11);  // Stop the stepper motor
  }
}

void serialprint(int measurepoint, int verticalPosition) {
  Serial.print(measurepoint + verticalPosition * measurementsPerTurn);  //print measurepoint
  Serial.print(",");
  if (forward) {
    Serial.print((measurepoint - 1) * (360 / measurementsPerTurn));  //print angle
  } else {
    Serial.print(360 - ((measurepoint) * (360 / measurementsPerTurn)));  //print angle
  }
  Serial.print(",");
  Serial.print(verticalPosition*distanceEnterDiameters);  //print diameter
  Serial.print(",");
  int power = measurePower();
  Serial.println(power);  //print power
}

int measurePower() {
  int power;
  //powermeasurement
  power = 10;
  return power;
}

void stoppStepper(int a, int b, int c, int d) {
  digitalWrite(a, LOW);
  digitalWrite(b, LOW);
  digitalWrite(c, LOW);
  digitalWrite(d, LOW);
}

void verticalMovement() {
  linearStepper.step(verticalSteps);  // Move the stepper motor
  stoppStepper(8, 9, 10, 11);
  delay(2000);
}