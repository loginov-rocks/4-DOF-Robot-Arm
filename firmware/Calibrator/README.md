# Calibrator

PlatformIO project for microcontrollers supporting Arduino framework for calibrating multiple servo motors using a
combination of hardware controls (button + potentiometer) and serial commands. While originally designed for
calibrating 4-DOF robot arms like [MeArm](https://github.com/MeArm/MeArm), it can be adapted for any project requiring
precise servo positioning and calibration.

## Features

- Control multiple servos with a single potentiometer and button.
- Direct servo control via serial commands.
- Debounced button input handling.
- Smooth potentiometer reading with adjustable deviation threshold.
- Servo position memory and reset functionality.
- Detailed serial feedback for all operations.

## Hardware Requirements

- Microcontroller supporting Arduino framework.
- Push button (connected to pin `2` by default): [scheme](https://docs.arduino.cc/built-in-examples/digital/Button/).
- Potentiometer (connected to pin `A0` by default):
  [scheme](https://docs.arduino.cc/learn/electronics/potentiometer-basics/).
- One or more servo motors (I use 4 x SG90 connected to pins `6`, `9`, `10`, and `11`):
  [scheme](https://docs.arduino.cc/tutorials/generic/basic-servo-control/).
- Appropriate power supply for servos (I use a 9V 3A AC/DC power supply with a power module for this setup).

## Usage

### Hardware Control

1. Click the button to cycle through the configured servos.
2. Turn the potentiometer to adjust the position of the currently selected servo (0-180 degrees).
3. Click through all servos to disengage control.

The system provides feedback via Serial (`9600` baud by default) for all operations.

### Serial Commands

The following commands are supported (send them via Serial at `9600` baud by default):

- `<servo_name> <angle>` - move a servo to an angle (from 0 to 180), for example:
  - `Base 10`
  - `Shoulder 20`
  - `Elbow 30`
  - `Claw 45`
- `RESET` - move all servos to default angle.

## Classes

### Button

A class for handling button input with debouncing:

```cpp
Button button(2);

button.setDebounceDelay(50); // Optional, default 50 ms

if (button.wasClicked()) {
  // Handle click.
}
```

### Potentiometer

A class for reading analog input with smoothing and threshold detection:

```cpp
Potentiometer potentiometer(A0);

potentiometer.setReadingDeviation(5); // Optional, default 5 units
potentiometer.setDebounceDelay(20);   // Optional, default 20 ms

if (potentiometer.wasChanged()) {
  int value = potentiometer.getValue();
  // Handle value change.
}
```

### Calibrator

Main class for managing multiple servos:

```cpp
Calibrator calibrator;

calibrator.attachServo(6, "Base", 90);

calibrator.cycleServo();         // Select next servo
calibrator.setAngle(45);         // Set current servo angle
calibrator.setAngle("Base", 90); // Set specific servo angle
calibrator.resetServos();        // Reset all servos to default
```

## Example Setup for 4-DOF Robot Arm

```cpp
#include <Arduino.h>

#include "Button.h"
#include "Calibrator.h"
#include "Potentiometer.h"

Button button(2);
Calibrator calibrator;
Potentiometer potentiometer(A0);

void setup() {
  Serial.begin(9600);

  calibrator.attachServo(6, "Base", 90);
  calibrator.attachServo(9, "Shoulder", 90);
  calibrator.attachServo(10, "Elbow", 90);
  calibrator.attachServo(11, "Claw", 25);
}
```

## Implementation Details

- Servo positions are tracked and maintained even when cycling through different servos.
- Potentiometer readings are mapped from 0-1023 to 0-180 degrees.
- Button debouncing prevents false triggers.
- Potentiometer has an adjustable deviation threshold to prevent unnecessary updates.
- Serial command parsing includes trim for reliable operation.
