#include <Servo.h>  // Include the Servo library to control servo motors

#define servosRows 5
#define servosColumns 3
#define removePushOut 3.5


// Create servo objects to control the motors
Servo myServo1, myServo2, myServo3,
myServo4,myServo5,myServo6,
myServo7,myServo8,myServo9,
myServo10,myServo11,myServo12,
myServo13,myServo14,myServo15;

// Pins connected to the servos' signal wires
// Pins #22-36
const int servoPin1 = 22; //2
const int servoPin2 = 23; //3
const int servoPin3 = 24; //4
const int servoPin4 = 25; //5
const int servoPin5 = 26; //6
const int servoPin6 = 27; //7
const int servoPin7 = 28; //8
const int servoPin8 = 29; //9
const int servoPin9 = 30; //10
const int servoPin10 = 31;  //11
const int servoPin11 = 32;  //12
const int servoPin12 = 33;  //13
const int servoPin13 = 34;  //14
const int servoPin14 = 35;  //15
const int servoPin15 = 36;  //16

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
  float currentHeight;
};

// Array of servo states for all three servos
//left-right from the starting position
//allows preset arrays to be written from left-right
//just trust me
ServoState servos[servosRows][servosColumns] = {
  {{&myServo3, 0, 0, false, 0,0.0},{&myServo2, 0, 0, false, 0,0.0},{&myServo1, 0, 0, false, 0,0.0}},
  {{&myServo6, 0, 0, false, 0,0.0},{&myServo5, 0, 0, false, 0,0.0},{&myServo4, 0, 0, false, 0,0.0}},
  {{&myServo9, 0, 0, false, 0,0.0},{&myServo8, 0, 0, false, 0,0.0},{&myServo7, 0, 0, false, 0,0.0}},
  {{&myServo12, 0, 0, false, 0,0.0},{&myServo11, 0, 0, false, 0,0.0},{&myServo10, 0, 0, false, 0,0.0}},
  {{&myServo15, 0, 0, false, 0,0.0},{&myServo14, 0, 0, false, 0,0.0},{&myServo13, 0, 0, false, 0,0.0}},
};

float leftRightPreset[servosRows][servosColumns] = {
  {0.8,0.6,0.3},
  {1.0,0.8,0.2},
  {1.1,1.0,0.4},
  {1.3,1.0,0.4},
  {1.5,1.0,0.4}
};
float rightLeftPreset[servosRows][servosColumns] = {
  {0.3,0.6,0.8},
  {0.2,0.8,1.0},
  {0.4,1.0,1.2},
  {0.4,1.0,1.3},
  {0.4,1.0,1.5}
};

float preset2[servosRows][servosColumns] = {
  {1.7,0.1,0.9},
  {0.7,1.1,0.5},
  {0.9,1.3,0.7},
  {0.9,1.5,0.7},
  {0.9,1.3,0.7}
};

float setupPreset[servosRows][servosColumns] = {
{-2.1,-1.88,-1.94}, // calibrated
{-1.7,-1.78,-1.82}, // calibrated
{-1.76,-2.05,-2.25}, // calibrated
{-2,-1.73,-1.87}, // calibrated
{-1.82,-1.78,-1.78} // calibrated
};

void setup() {
  // Initialize the serial communication
  Serial.begin(9600);

  // Attach the servo objects to their respective pins
  myServo1.attach(servoPin1);
  myServo2.attach(servoPin2);
  myServo3.attach(servoPin3);
  myServo4.attach(servoPin4);
  myServo5.attach(servoPin5);
  myServo6.attach(servoPin6);
  myServo7.attach(servoPin7);
  myServo8.attach(servoPin8);
  myServo9.attach(servoPin9);
  myServo10.attach(servoPin10);
  myServo11.attach(servoPin11);
  myServo12.attach(servoPin12);
  myServo13.attach(servoPin13);
  myServo14.attach(servoPin14);
  myServo15.attach(servoPin15);

  // // Inform the user about the input format
  // Serial.println("Enter the desired rack movement for multiple servos:");
  // Serial.println("Format: servoNumber distance, servoNumber distance (e.g., 1 2.5, 2 -1.0)");
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
  for (int i = 0; i <servosRows; i++) {
    for (int j =0; j <servosColumns; j++){
      updateServo(servos[i][j]);
    }
  }
}

// Function to process a single preset
void processCommand(String command) {
  /*=================BEGIN COMMANDS==========================*/
  if (command == "lr"){
    for(int i = 0; i < servosRows; i++){
      for(int j = 0; j < servosColumns; j++){
        moveRack(servos[i][j], leftRightPreset[i][j] - servos[i][j].currentHeight);
      }
    }
    Serial.println(1);
  }
if (command == "setup"){
    for(int i = 0; i < servosRows; i++){
      for(int j = 0; j < servosColumns; j++){
        moveRack(servos[i][j], setupPreset[i][j]);
        servos[i][j].currentHeight = 0;
      }
    }
    Serial.println(0);
  }
    if (command == "remove"){
    for(int i = 0; i < servosRows; i++){
      for(int j = 0; j < servosColumns; j++){
        moveRack(servos[i][j], removePushOut);
        servos[i][j].currentHeight = 0;
      }
    }
    Serial.println(-1);
  }

  if (command == "p2"){
    for(int i = 0; i < servosRows; i++){
      for(int j = 0; j < servosColumns; j++){
        moveRack(servos[i][j], preset2[i][j] - servos[i][j].currentHeight);
      }
    }
    Serial.println(2);
  }

  if (command == "rl"){
    for(int i = 0; i < servosRows; i++){
      for(int j = 0; j < servosColumns; j++){
        moveRack(servos[i][j], rightLeftPreset[i][j] - servos[i][j].currentHeight);
      }
    }
    Serial.println(3);
  }
  if (command == "flat"){
    for(int i = 0; i < servosRows; i++){
      for(int j = 0; j < servosColumns; j++){
        moveRack(servos[i][j], -servos[i][j].currentHeight);
      }
    }
    Serial.println(4);
  }
  if (command == "testservo"){
    moveRack(servos[5][3],removePushOut);
    delay(100);
    moveRack(servos[5][3], setupPreset[5][3]);
    Serial.println(99);
  }
/*=====================END COMMANDS====================*/
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
  servoState.currentHeight = servoState.currentHeight + distance;
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
