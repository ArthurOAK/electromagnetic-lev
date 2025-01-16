// Pin Definitions
const int smallStepDirPin = 4;
const int smallStepPulsePin = 3;
const int bigStepEnablePin = 12;
const int bigStepDirPin = 11;
const int bigStepPulsePin = 10;

// Motor Parameters
const int stepsPerRev = 3200; // Steps per revolution for stepper motors
const float smallMotorStepsPerBigMotorRev = 7.5; // Steps for small motor per big motor revolution
const int turnsPerLayer = 172; // Big motor turns per layer
const int totalLayers = 1;     // For testing, set to 1 layer
volatile int currentBigSteps = 0;
volatile int currentSmallSteps = 0;
volatile int bigRevolutions = 0;
volatile int layerCompleted = 0;

// Direction Flags
bool smallMotorDirection = LOW; // Start with counterclockwise for wireMotor
bool bigMotorDirection = HIGH;  // Clockwise for coreMotor

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

  // Enable Big Motor
  digitalWrite(bigStepEnablePin, HIGH);
  digitalWrite(bigStepDirPin, bigMotorDirection);
  digitalWrite(smallStepDirPin, smallMotorDirection);

  // Setup Timers
  setupTimers();

  // Initial Log
  Serial.begin(9600);
  Serial.println("Initialization complete. Motors will start shortly...");
}

// Main Loop
void loop() {
  // Logic for layers and stopping the motors
  if (layerCompleted >= totalLayers) {
    stopMotors();
    Serial.println("All layers completed. Program stopped.");
    while (true); // Infinite loop to halt the program
  }

  // Update direction for the small motor at layer transitions
  if (bigRevolutions == turnsPerLayer) {
    bigRevolutions = 0;
    smallMotorDirection = !smallMotorDirection; // Reverse direction
    digitalWrite(smallStepDirPin, smallMotorDirection);
    layerCompleted++;
    Serial.print("Layer completed: ");
    Serial.println(layerCompleted);
  }
}

// Timer Interrupt Setup
void setupTimers() {
  // Configure Timer1 for Big Motor
  noInterrupts();            // Disable interrupts
  TCCR1A = 0;                // Clear Timer/Counter Control Register A
  TCCR1B = 0;                // Clear Timer/Counter Control Register B
  TCNT1 = 0;                 // Initialize Timer1 counter to 0
  OCR1A = 400;               // Set compare match register for 25 kHz (16 MHz / (2 * 400 * 8))
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
  // Toggle Pulse State
  bigMotorStepState = !bigMotorStepState;
  digitalWrite(bigStepPulsePin, bigMotorStepState);

  // Count Steps
  if (bigMotorStepState) {
    currentBigSteps++;
    if (currentBigSteps >= stepsPerRev) { // One revolution completed
      currentBigSteps = 0;
      bigRevolutions++;
    }
  }
}

// Timer2 Interrupt for Small Motor
ISR(TIMER2_COMPA_vect) {
  if (currentBigSteps % int(stepsPerRev / smallMotorStepsPerBigMotorRev) == 0) {
    // Toggle Pulse State
    smallMotorStepState = !smallMotorStepState;
    digitalWrite(smallStepPulsePin, smallMotorStepState);

    // Count Steps
    if (smallMotorStepState) {
      currentSmallSteps++;
    }
  }
}

// Stop All Motors
void stopMotors() {
  digitalWrite(bigStepEnablePin, LOW);
  digitalWrite(bigStepPulsePin, LOW);
  digitalWrite(smallStepPulsePin, LOW);
}
