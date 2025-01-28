#include <Servo.h>  // Include the Servo library to control servo motors

// Create servo objects to control the motors
Servo myServo1, myServo2, myServo3;

// Pins connected to the servos' signal wires
const int servoPin1 = 9;  
const int servoPin2 = 10;
const int servoPin3 = 11;

// Pinion-related constants
const float pinionCircumference = 3.14159;  // Pinion circumference in inches (1-inch diameter)

// Servo control and timing constants
const int forwardSpeed = 180;  // Servo signal value for full forward rotation
const int stopSpeed = 90;  // Servo signal value to stop the servo
const float servoSpeedRPM = 30;  // Servo rotational speed in rotations per minute (datasheet)

// Structure to track each servo's movement state
struct ServoState {
  Servo* servo;  // Pointer to the servo object
  float remainingTime;  // Remaining movement time in milliseconds
  unsigned long startTime;  // Start time of the movement
  bool active;  // Indicates whether the servo is actively moving
  int direction;  // Movement direction (1 for forward, -1 for backward, 0 for stop)
};

// Array of servo states for all three servos
ServoState servos[] = {
  {&myServo1, 0, 0, false, 0},
  {&myServo2, 0, 0, false, 0},
  {&myServo3, 0, 0, false, 0},
};

void setup() {
  // Initialize the serial communication
  Serial.begin(9600);

  // Attach the servo objects to their respective pins
  myServo1.attach(servoPin1);
  myServo2.attach(servoPin2);
  myServo3.attach(servoPin3);

  // Inform the user about the input format
  Serial.println("Enter the desired rack movement for multiple servos:");
  Serial.println("Format: servoNumber distance, servoNumber distance (e.g., 1 2.5, 2 -1.0)");
}

void loop() {
  // Check if there is serial input available
  if (Serial.available() > 0) {
    // Read the input as a string
    String input = Serial.readStringUntil('\n');
    input.trim();  // Remove any leading/trailing whitespace

    // Split the input by commas to separate multiple commands
    while (input.length() > 0) {
      int commaIndex = input.indexOf(',');
      String command;
      if (commaIndex != -1) {
        // Extract the next command and update the input
        command = input.substring(0, commaIndex);
        input = input.substring(commaIndex + 1);
      } else {
        // Last command
        command = input;
        input = "";
      }
      command.trim();  // Remove any extra spaces
      processCommand(command);  // Process the command
    }
  }

  // Update all servos
  for (int i = 0; i < 3; i++) {
    updateServo(servos[i]);
  }
}

// Function to process a single command like "1 2.5"
void processCommand(String command) {
  // Parse the servo number and target distance
  int spaceIndex = command.indexOf(' ');
  if (spaceIndex == -1) {
    Serial.println("Invalid command format. Use: servoNumber distance");
    return;
  }
  int servoNumber = command.substring(0, spaceIndex).toInt();
  float targetRackMovement = command.substring(spaceIndex + 1).toFloat();

  // Validate the servo number and move the corresponding servo
  if (servoNumber >= 1 && servoNumber <= 3) {
    moveRack(servos[servoNumber - 1], targetRackMovement);  // Move the selected servo
    // Inform the user the operation is initiated
    Serial.print("Servo ");
    Serial.print(servoNumber);
    Serial.print(" is moving the rack by ");
    Serial.print(targetRackMovement);
    Serial.println(" inches.");
  } else {
    Serial.println("Invalid servo number. Please enter 1, 2, or 3.");
  }
}

// Function to initialize the movement of a servo
void moveRack(ServoState& servoState, float distance) {
  // Calculate the number of pinion rotations needed to achieve the desired rack movement
  float rotations = abs(distance) / pinionCircumference;

  // Convert rotations to the required duration in milliseconds
  servoState.remainingTime = (rotations * 60.0 / servoSpeedRPM) * 1000;

  // Determine the direction of rotation
  if (distance > 0) {
    servoState.direction = 1;  // Forward
    servoState.servo->write(forwardSpeed);  // Set the servo to full forward rotation
  } else if (distance < 0) {
    servoState.direction = -1;  // Backward
    servoState.servo->write(180 - forwardSpeed);  // Set the servo to backward rotation
  } else {
    servoState.direction = 0;  // Stop
    servoState.servo->write(stopSpeed);  // Stop the servo
    return;
  }

  // Mark the servo as active
  servoState.active = true;
  servoState.startTime = millis();  // Record the start time
}

// Function to update the state of a servo
void updateServo(ServoState& servoState) {
  if (!servoState.active) {
    return;  // Skip if the servo is not active
  }

  // Calculate the elapsed time
  unsigned long currentTime = millis();
  unsigned long elapsedTime = currentTime - servoState.startTime;

  // Update remaining time
  servoState.remainingTime -= elapsedTime;
  servoState.startTime = currentTime;

  // Stop the servo if the movement is complete
  if (servoState.remainingTime <= 0) {
    servoState.servo->write(stopSpeed);  // Stop the servo
    servoState.active = false;  // Mark the servo as inactive
  }
}
