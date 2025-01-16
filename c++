// Pin Definitions
const int smallStepDirPin = 4;
const int smallStepPulsePin = 3;
const int bigStepEnablePin = 12;
const int bigStepDirPin = 11;
const int bigStepPulsePin = 10;

// Motor Parameters
const int bigMotorStepsPerRev = 3200;  // Steps per revolution for the big motor
const int smallMotorStepsPerRev = 200; // Steps per revolution for the small motor
const float smallStepsPerBigRev = 7.5; // Small motor steps per big motor revolution
const int bigMotorTurnsPerLayer = 172; // Big motor turns per layer
const int totalLayers = 1;             // Total layers for testing

// Motion Control Parameters
const int maxBigMotorSpeed = 500; // Maximum speed in microseconds per step
const int minBigMotorSpeed = 2000; // Minimum speed (start/stop speed) in microseconds per step
const int accelerationStepDelay = 2; // Decrease step delay in microseconds per step (acceleration rate)

// Direction Flags
bool smallMotorDirection = LOW; // Initial direction of the small motor
bool bigMotorDirection = HIGH;  // Initial direction of the big motor

// Motion Tracking Variables
int currentBigSteps = 0;
float currentSmallSteps = 0;
int bigRevolutions = 0;
int layerCompleted = 0;

// Setup Function
void setup() {
  // Pin Setup
  pinMode(smallStepDirPin, OUTPUT);
  pinMode(smallStepPulsePin, OUTPUT);
  pinMode(bigStepEnablePin, OUTPUT);
  pinMode(bigStepDirPin, OUTPUT);
  pinMode(bigStepPulsePin, OUTPUT);

  // Enable Motors and Set Initial Directions
  digitalWrite(bigStepEnablePin, HIGH);
  digitalWrite(bigStepDirPin, bigMotorDirection);
  digitalWrite(smallStepDirPin, smallMotorDirection);

  // Initialize Serial Communication
  Serial.begin(9600);
  Serial.println("Initialization complete. Motors will start shortly...");

  // Test Rotations
  testMotors();
}

// Function to Gradually Rotate the Motors for Smooth Motion
void rotateBigMotorSmoothly(int steps, bool direction) {
  digitalWrite(bigStepDirPin, direction); // Set direction
  int stepDelay = minBigMotorSpeed;       // Start with the minimum speed

  // Gradually accelerate, maintain speed, and decelerate
  for (int i = 0; i < steps; i++) {
    // Toggle pulse pin
    digitalWrite(bigStepPulsePin, HIGH);
    delayMicroseconds(stepDelay);
    digitalWrite(bigStepPulsePin, LOW);
    delayMicroseconds(stepDelay);

    // Accelerate or decelerate
    if (i < steps / 2 && stepDelay > maxBigMotorSpeed) {
      stepDelay -= accelerationStepDelay; // Accelerate
    } else if (i > steps / 2 && stepDelay < minBigMotorSpeed) {
      stepDelay += accelerationStepDelay; // Decelerate
    }

    // Track steps and revolutions
    currentBigSteps++;
    if (currentBigSteps >= bigMotorStepsPerRev) {
      currentBigSteps = 0;
      bigRevolutions++;
    }

    // Synchronize the small motor
    if (i % int(bigMotorStepsPerRev / smallStepsPerBigRev) == 0) {
      rotateSmallMotorSmoothly(1, smallMotorDirection);
    }
  }
}

// Function to Rotate the Small Motor Smoothly
void rotateSmallMotorSmoothly(int steps, bool direction) {
  digitalWrite(smallStepDirPin, direction); // Set direction
  int stepDelay = minBigMotorSpeed;         // Start with the minimum speed

  // Smoothly rotate the small motor
  for (int i = 0; i < steps; i++) {
    digitalWrite(smallStepPulsePin, HIGH);
    delayMicroseconds(stepDelay);
    digitalWrite(smallStepPulsePin, LOW);
    delayMicroseconds(stepDelay);

    // Track steps
    currentSmallSteps += (direction == LOW) ? -1 : 1;
  }
}

// Function to Test Motors with Smooth Rotation
void testMotors() {
  // Smoothly rotate the big motor for 1 revolution
  Serial.println("Testing big motor for 1 revolution...");
  rotateBigMotorSmoothly(bigMotorStepsPerRev, bigMotorDirection);

  // Smoothly rotate the small motor for 1 revolution
  Serial.println("Testing small motor for 1 revolution...");
  rotateSmallMotorSmoothly(smallMotorStepsPerRev, smallMotorDirection);

  Serial.println("Motor testing complete.");
}

// Main Loop
void loop() {
  // Layer completion logic
  if (layerCompleted >= totalLayers) {
    stopMotors();
    Serial.println("All layers completed. Program stopped.");
    while (true); // Halt the program
  }

  // Rotate the big motor smoothly for a full layer
  if (bigRevolutions < bigMotorTurnsPerLayer) {
    rotateBigMotorSmoothly(bigMotorStepsPerRev, bigMotorDirection);
  } else {
    // Layer completed, reverse small motor direction
    bigRevolutions = 0;
    layerCompleted++;
    smallMotorDirection = !smallMotorDirection;
    digitalWrite(smallStepDirPin, smallMotorDirection);
  }
}

// Function to Stop All Motors
void stopMotors() {
  digitalWrite(bigStepEnablePin, LOW);
  digitalWrite(bigStepPulsePin, LOW);
  digitalWrite(smallStepPulsePin, LOW);
}
