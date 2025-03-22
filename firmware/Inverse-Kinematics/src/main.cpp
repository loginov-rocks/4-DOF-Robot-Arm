#include <Arduino.h>
#include <meArm.h>
// #include <MeArm.h>

meArm arm(128, 33, -pi / 4, pi / 4,    // Base
          135, 35, pi / 4, 3 * pi / 4, // Shoulder
          135, 45, pi / 4, -pi / 4,    // Elbow
          14, 41, pi / 2, 0);          // Claw
// MeArm arm(...);

String readBuffer = "";

String getInput();
boolean isCoordinateCommand(String);
void executeCoordinateCommand(String);

void setup()
{
  Serial.begin(9600);

  // Base, Shoulder, Elbow, Claw pins.
  arm.begin(6, 9, 10, 11);

  Serial.println("Setup complete");
  Serial.println();
}

void loop()
{
  String input = getInput();

  if (input.length() > 0)
  {
    if (isCoordinateCommand(input))
    {
      executeCoordinateCommand(input);
    }
    else if (input == "c")
    {
      Serial.println("Opening the claw");
      arm.openGripper();
      // arm.openClaw();
    }
    else if (input == "C")
    {
      Serial.println("Closing the claw");
      arm.closeGripper();
      // arm.closeClaw();
    }
    else if (input == "O")
    {
      Serial.println("Going to the home point");
      arm.gotoPoint(0.0, 100.0, 50.0);
      // arm.moveToXYZ(0, 100, 50);
    }
  }
}

String getInput()
{
  String input = "";

  while (Serial.available())
  {
    char c = Serial.read();

    if (c == '\n')
    {
      input = readBuffer;
      input.trim();
      readBuffer = "";
    }
    else if (c)
    {
      readBuffer += c;
    }
  }

  return input;
}

boolean isCoordinateCommand(String command)
{
  return (command[0] == 'X' || command[0] == 'Y' || command[0] == 'Z');
}

void executeCoordinateCommand(String command)
{
  float x = arm.getX(),
        y = arm.getY(),
        z = arm.getZ();

  // Get float value from command string after the first character.
  float val = command.substring(1).toFloat();

  // Determine what coordinate will be changed.
  switch (command[0])
  {
  case 'X':
    x = val;
    break;

  case 'Y':
    y = val;
    break;

  case 'Z':
    z = val;
    break;

  default:
    return;
  }

  Serial.print("Going to the point (");
  Serial.print(x);
  Serial.print(", ");
  Serial.print(y);
  Serial.print(", ");
  Serial.print(z);
  Serial.print(")");

  if (arm.isReachable(x, y, z))
  {
    Serial.println();
  }
  else
  {
    Serial.println(", which is not reachable");
  }

  arm.gotoPoint(x, y, z);
  // arm.moveToXYZ(x, y, z);
}
