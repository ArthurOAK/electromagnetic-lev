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

// Step Tracking
volatile int currentBigSteps = 0;       // Steps completed by the big motor
volatile float currentSmallSteps = 0;  // Steps completed by the small motor
volatile int bigRevolutions = 0;       // Number of big motor revolutions
volatile int layerCompleted = 0;       // Number of completed layers

// Direction Flags
bool smallMotorDirection = LOW; // Initial direction of the small motor
bool bigMotorDirection = HIGH;  // Initial direction of the big motor

// Timer Interrupt Variables
volatile bool bigMotorStepState = false;
volatile bool smallMotorStepState = false;

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

  // Setup Timers
  setupTimers();
}

// Function to Test Motors with 1 Revolution Each
void testMotors() {
  // Rotate Big Motor for 1 Revolution
  for (int i = 0; i < bigMotorStepsPerRev; i++) {
    digitalWrite(bigStepPulsePin, HIGH);
    delayMicroseconds(500);
    digitalWrite(bigStepPulsePin, LOW);
    delayMicroseconds(500);
  }

  // Rotate Small Motor for 1 Revolution
  for (int i = 0; i < smallMotorStepsPerRev; i++) {
    digitalWrite(smallStepPulsePin, HIGH);
    delayMicroseconds(500);
    digitalWrite(smallStepPulsePin, LOW);
    delayMicroseconds(500);
  }
}

// Main Loop
void loop() {
  // Stop all motors after completing all layers
  if (layerCompleted >= totalLayers) {
    stopMotors();
    Serial.println("All layers completed. Program stopped.");
    while (true); // Halt the program
  }
}

// Timer Interrupt Setup
void setupTimers() {
  // Configure Timer1 for Big Motor
  noInterrupts();            // Disable interrupts
  TCCR1A = 0;                // Clear Timer/Counter Control Register A
  TCCR1B = 0;                // Clear Timer/Counter Control Register B
  TCNT1 = 0;                 // Initialize Timer1 counter to 0
  OCR1A = 400;               // Set compare match register for ~25 kHz
  TCCR1B |= (1 << WGM12);    // Configure Timer1 in CTC mode
  TCCR1B |= (1 << CS11);     // Prescaler 8
  TIMSK1 |= (1 << OCIE1A);   // Enable Timer1 compare interrupt

  // Configure Timer2 for Small Motor
  TCCR2A = 0;
  TCCR2B = 0;
  TCNT2 = 0;
  OCR2A = 50;                // Adjusted for smaller delays for small motor
  TCCR2A |= (1 << WGM21);    // Configure Timer2 in CTC mode
  TCCR2B |= (1 << CS21);     // Prescaler 8
  TIMSK2 |= (1 << OCIE2A);   // Enable Timer2 compare interrupt
  interrupts();              // Enable interrupts
}

// Timer1 Interrupt for Big Motor
ISR(TIMER1_COMPA_vect) {
  bigMotorStepState = !bigMotorStepState;
  digitalWrite(bigStepPulsePin, bigMotorStepState);

  if (bigMotorStepState) {
    currentBigSteps++;
    if (currentBigSteps >= bigMotorStepsPerRev) {
      currentBigSteps = 0;
      bigRevolutions++;
      if (bigRevolutions % bigMotorTurnsPerLayer == 0) {
        // Layer Completed
        layerCompleted++;
        smallMotorDirection = !smallMotorDirection; // Reverse small motor direction
        digitalWrite(smallStepDirPin, smallMotorDirection);
      }
    }
  }
}

// Timer2 Interrupt for Small Motor
ISR(TIMER2_COMPA_vect) {
  if (currentBigSteps % int(bigMotorStepsPerRev / smallStepsPerBigRev) == 0) {
    smallMotorStepState = !smallMotorStepState;
    digitalWrite(smallStepPulsePin, smallMotorStepState);

    if (smallMotorStepState) {
      currentSmallSteps += (smallMotorDirection == LOW) ? -1 : 1;
    }
  }
}

// Function to Stop All Motors
void stopMotors() {
  digitalWrite(bigStepEnablePin, LOW);
  digitalWrite(bigStepPulsePin, LOW);
  digitalWrite(smallStepPulsePin, LOW);
}
