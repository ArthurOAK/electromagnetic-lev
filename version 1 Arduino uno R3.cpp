
// Define constants and variables for wire and core parameters
float wireDiameter, coreDiameter, wiringDistance;
int TurnsPerLayer = 172;   // Total turns for one layer
int currentLayer = 0;      // Track current layer
int maxLayers = 23;        // Maximum number of layers
int NumberOfTurns = 4040;  // Total number of turns

// Define pins for stepper motors
const int smallStepDirPin = 4;
const int smallStepPulsePin = 3;
const int bigStepEnablePin = 12;
const int bigStepDirPin = 11;
const int bigStepPulsePin = 10;

// Define steps per revolution and delay
const int stepsPerRev = 3200; // step/rev
const int bigStepDelay = 1000; // delay time for big stepper motor in microseconds

void setup() {
  // Set pin modes for small and big stepper motors
  pinMode(smallStepDirPin, OUTPUT);  
  pinMode(smallStepPulsePin, OUTPUT); 
  pinMode(bigStepEnablePin, OUTPUT);  
  pinMode(bigStepDirPin, OUTPUT);  
  pinMode(bigStepPulsePin, OUTPUT);  
}

void loop() {
  wireDiameter = 0.15;  // Wire diameter in mm
  coreDiameter = 0.790;  // Core diameter in mm
  wiringDistance = 45;  // Wiring distance in mm

  int numberOfLayers = NumberOfTurns / TurnsPerLayer;
  if (currentLayer >= numberOfLayers) {
    // Stop when it reaches the number of layers
    while (true); // stop the program (replace with desired behavior)
  }

  // Calculate the number of big motor steps required for the small motor to move 1 step
  int stepsForSmallMotor = 4 / wireDiameter; // The big motor must complete 4/Dw steps before the small motor takes 1 step

  // Loop for each turn in the current layer
  for (int i = 0; i < TurnsPerLayer; i++) { 
    // Enable the big stepper motor
    digitalWrite(bigStepEnablePin, HIGH);

    int bigStepCounter = 0; // Counter to track big motor steps

    // Loop for the full revolution of the big motor
    for (int bigStep = 0; bigStep < stepsPerRev; bigStep++) {
      // Control the big (core) stepper motor
      digitalWrite(bigStepDirPin, HIGH); // direction: clockwise
      digitalWrite(bigStepPulsePin, HIGH);  // Big motor pulse pin
      delayMicroseconds(bigStepDelay);
      digitalWrite(bigStepPulsePin, LOW);
      delayMicroseconds(bigStepDelay);

      bigStepCounter++; 

      // If the big motor has taken enough steps, move the small motor
      if (bigStepCounter >= stepsForSmallMotor) {
        // Reset the counter
        bigStepCounter = 0;

        // Control the small (wire feed) stepper motor
        digitalWrite(smallStepDirPin, HIGH); // direction: forward
        digitalWrite(smallStepPulsePin, HIGH); // Small motor pulse pin
        digitalWrite(smallStepPulsePin, LOW);
      }
    }
  }

  currentLayer++; // Move to the next layer after completing one layer
  if (currentLayer >= numberOfLayers) {
    // Stop when it reaches the number of layers
    while (true); // stop the program (replace with desired behavior)
  }

for (int i = 0; i < TurnsPerLayer; i++) { 
    // Enable the big stepper motor
    digitalWrite(bigStepEnablePin, HIGH);

    int bigStepCounter = 0; // Counter to track big motor steps

    // Loop for the full revolution of the big motor
    for (int bigStep = 0; bigStep < stepsPerRev; bigStep++) {
      // Control the big (core) stepper motor
      digitalWrite(bigStepDirPin, HIGH); // direction: clockwise
      digitalWrite(bigStepPulsePin, HIGH);  // Big motor pulse pin
      delayMicroseconds(bigStepDelay);
      digitalWrite(bigStepPulsePin, LOW);
      delayMicroseconds(bigStepDelay);

      bigStepCounter++; 

      // If the big motor has taken enough steps, move the small motor
      if (bigStepCounter >= stepsForSmallMotor) {
        // Reset the counter
        bigStepCounter = 0;

        // Control the small (wire feed) stepper motor
        digitalWrite(smallStepDirPin, LOW); // direction: forward
        digitalWrite(smallStepPulsePin, HIGH); // Small motor pulse pin
        digitalWrite(smallStepPulsePin, LOW);
      }
    }
  }
}