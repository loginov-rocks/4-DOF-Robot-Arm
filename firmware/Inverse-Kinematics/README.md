# Inverse Kinematics

Note: as it's implemented in the library, moving the arm using cartesian coordinates will not update cylindrical,
so when sending cylindrical coordinate after moving in the cartesian system, it will default to the previously used
cylindrical. The opposite is not true, since updating cylindrical coordinates use cartesian under the hood, so
cartesian coordinates will always be updated. Example:

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
