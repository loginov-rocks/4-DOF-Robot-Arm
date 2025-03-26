# Inverse Kinematics

PlatformIO project for microcontrollers supporting Arduino framework that controls a
[MeArm](https://github.com/MeArm/MeArm) robot arm using inverse kinematics. This allows precise arm movement in 3D
space using either Cartesian (X, Y, Z) or Cylindrical (T, R, Z) coordinate systems via serial commands.

# Features

- Control MeArm robot arm movements using inverse kinematics.
- Support for both Cartesian and Cylindrical coordinate systems.
- Serial command interface for precise control.
- Position reporting in both coordinate systems.
- Claw open/close functionality.
- Detailed serial feedback for all operations.
- Compatible with both the York Hackspace team library (MeArm v0.4 or v1.0) and the official MeArm library (v3.0).

## Hardware Requirements

- Microcontroller supporting Arduino framework.
- One or more servo motors (I use 4 x SG90 connected to pins `6`, `9`, `10`, and `11`):
  [scheme](https://docs.arduino.cc/tutorials/generic/basic-servo-control/).
- Appropriate power supply for servos (I use a 9V 3A AC/DC power supply with a power module for this setup).

## Usage

1. Connect the MeArm servos to the specified pins.
2. Upload the code to your microcontroller.
3. Open a serial monitor at `9600` baud.
4. Send commands to control the arm's position and claw.

### Library Support

This project is compatible with two different libraries implementing inverse kinematics for MeArm:

1. Original York Hackspace library: works with MeArm v0.4 and v1.0 (enabled by default).
2. Official MeArm library: configured for MeArm v3.0.

The code contains marked comments (in
[platformio.ini](https://github.com/loginov-rocks/4-DOF-Robot-Arm/blob/main/firmware/Inverse-Kinematics/platformio.ini) and
[main.cpp](https://github.com/loginov-rocks/4-DOF-Robot-Arm/blob/main/firmware/Inverse-Kinematics/src/main.cpp)) indicating which lines to comment and uncomment when switching between libraries.

### Serial Commands

The following commands are supported (send them via Serial at `9600` baud by default):

- `<dimension> <coordinate>` - move the arm along the specified dimension, for example:
  - Cartesian coordinates:
    - `X 0` - left/right,
    - `Y 100` - forward/backward,
    - `Z 50` - up/down,
  - Cylindrical coordinates:
    - `T 0` - base rotation angle in degrees,
    - `R 100` - radius from the center,
    - note: `Z` command moves the arm in cartesian coordinates,
- `OPEN` - open the claw,
- `CLOSE` - close the claw,
- `CARTESIAN` - report current cartesian coordinates,
- `CYLINDRICAL` - report current cylindrical coordinates,
- `RESET` - move the arm to default coordinates.

## Example Setup for MeArm v1.0

```cpp
#include <Arduino.h>
#include <meArm.h>

// @see https://loginov-rocks.medium.com/calibrate-4-dof-robot-arm-mearm-for-inverse-kinematics-part-2-16698b674694
meArm arm(128, 33, -pi / 4, pi / 4,    // Base
          135, 35, pi / 4, 3 * pi / 4, // Shoulder
          135, 45, pi / 4, -pi / 4,    // Elbow
          14, 41, pi / 2, 0);          // Claw

void setup()
{
  Serial.begin(9600);
  // Base, Shoulder, Elbow, Claw pins.
  arm.begin(6, 9, 10, 11);
}
```

## Implementation Details

Libraries have an asymmetry in how they handle coordinate updates:

- **Cartesian → Cylindrical:** when you move the arm using Cartesian coordinates (X, Y, Z), the internal cylindrical
  coordinates (T, R, Z) are _not_ automatically updated. If you request cylindrical coordinates after a Cartesian
  movement, you'll receive the previous cylindrical values, not the actual current position.
- **Cylindrical → Cartesian:** conversely, when you move using cylindrical coordinates, the Cartesian coordinates _are_
  properly updated. This occurs because cylindrical movements use Cartesian calculations internally.

This behavior is important to consider when alternating between coordinate systems during operation. Example:

```
CARTESIAN
< CARTESIAN
Arm cartesian coordinates: X = 0, Y = 100, Z = 50

CYLINDRICAL
< CYLINDRICAL
Arm cylindrical coordinates: T = 0°, R = 100, Z = 50

X 50
< X 50
Moving arm to X = 50
Arm moved to cartesian coordinates: X = 50, Y = 100, Z = 50

CYLINDRICAL
< CYLINDRICAL
Arm cylindrical coordinates: T = 0°, R = 100, Z = 50
```
