#include <Arduino.h>
#include <meArm.h>
// #include <MeArm.h>

meArm arm(128, 33, -pi / 4, pi / 4,    // Base
          135, 35, pi / 4, 3 * pi / 4, // Shoulder
          135, 45, pi / 4, -pi / 4,    // Elbow
          14, 41, pi / 2, 0);          // Claw
// MeArm arm(...);

// readSerial helpers.
const size_t readSerialBufferSize = 16; // Should be large enough to accommodate a command.
char readSerialBuffer[readSerialBufferSize];
size_t readSerialBufferIndex = 0;

char *readSerial();
void moveCartesian(char, int);

void setup()
{
  Serial.begin(9600);

  Serial.println("Setting up...");
  Serial.println();

  // Base, Shoulder, Elbow, Claw pins.
  arm.begin(6, 9, 10, 11);

  Serial.println("Setup complete");
  Serial.println();
  Serial.println("Serial commands supported:");
  Serial.println("  <axis> <coordinate> - move the arm to coordinate, for example:");
  Serial.println("    X 0");
  Serial.println("    Y 100");
  Serial.println("    Z 50");
  Serial.println("  OPEN - open the claw");
  Serial.println("  CLOSE - close the claw");
  Serial.println("  RESET - move the arm to default coordinates");
  Serial.println();
}

void loop()
{
  // React to Serial input.
  char *input = readSerial();

  if (input != nullptr)
  {
    Serial.print("< ");
    Serial.println(input);

    if (strcmp(input, "OPEN") == 0)
    {
      arm.openGripper();
      // arm.openClaw();
      Serial.println("Claw opened");
    }
    else if (strcmp(input, "CLOSE") == 0)
    {
      arm.closeGripper();
      // arm.closeClaw();
      Serial.println("Claw closed");
    }
    else if (strcmp(input, "RESET") == 0)
    {
      arm.gotoPoint(0, 100, 50);
      // arm.moveToXYZ(0, 100, 50);
      Serial.println("Arm moved to default coordinates: X = 0, Y = 100, Z = 50");
    }
    else
    {
      char axis; // Should be large enough to accommodate the axis name.
      int coordinate;

      if (sscanf(input, "%c %d", &axis, &coordinate) == 2)
      {
        if (axis == 'X' || axis == 'Y' || axis == 'Z')
        {
          Serial.print("Moving arm to ");
          Serial.print(axis);
          Serial.print(" = ");
          Serial.println(coordinate);

          moveCartesian(axis, coordinate);
        }
        else
        {
          Serial.println("Unknown axis");
        }
      }
      else
      {
        Serial.println("Unknown command");
      }
    }

    Serial.println();
  }
}

/**
 * Reads characters from the serial input, ignoring leading and trimming trailing spaces. The function returns a
 * null-terminated string of characters received up to the first newline character ('\n'). If no data is available or
 * the input buffer is empty, it returns a null pointer.
 */
char *readSerial()
{
  while (Serial.available())
  {
    char inputChar = Serial.read();

    // Ignore leading spaces.
    if (readSerialBufferIndex == 0 && isspace(inputChar))
    {
      continue;
    }

    Serial.print(inputChar);

    // Exit when a new line is received.
    if (inputChar == '\n')
    {
      // Trim trailing spaces.
      while (readSerialBufferIndex > 0 && isspace(readSerialBuffer[readSerialBufferIndex - 1]))
      {
        readSerialBufferIndex--;
      }

      // Null-terminate the buffer and reset the index.
      readSerialBuffer[readSerialBufferIndex] = '\0';
      readSerialBufferIndex = 0;

      return readSerialBuffer;
    }

    if (readSerialBufferIndex < readSerialBufferSize - 1)
    {
      readSerialBuffer[readSerialBufferIndex++] = inputChar;
    }
  }

  return nullptr;
}

void moveCartesian(char axis, int coordinate)
{
  // Get current coordinates as the library moving function requires all coordinates to be passed when invoking.
  float x = arm.getX(),
        y = arm.getY(),
        z = arm.getZ();

  switch (axis)
  {
  case 'X':
    x = coordinate;
    break;
  case 'Y':
    y = coordinate;
    break;
  case 'Z':
    z = coordinate;
    break;
  default:
    Serial.println("Unknown axis");
    return;
  }

  arm.gotoPoint(x, y, z);
  // arm.moveToXYZ(x, y, z);

  // Note: the library will still attempt to move to the specified coordinates even when isReachable() returns false.
  bool isReachable = arm.isReachable(x, y, z);

  Serial.print("Arm ");
  if (isReachable)
  {
    Serial.print("moved");
  }
  else
  {
    Serial.print("tried to move");
  }
  Serial.print(" to coordinates: X = ");
  Serial.print(x);
  Serial.print(", Y = ");
  Serial.print(y);
  Serial.print(", Z = ");
  Serial.print(z);
  if (isReachable)
  {
    Serial.println();
  }
  else
  {
    Serial.println(" - but these were calculated as unreachable");
  }
}
