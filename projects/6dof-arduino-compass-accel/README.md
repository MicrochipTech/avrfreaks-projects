# 6DOF Arduino: Compass & Accel...

Uploaded by krazatchu on 2012-02-27 12:20:00 (rating 0 out of 5)

## Summary

This is a mixed bag library for compass and accelerometer fusing...  

The three main parts are:


 1) Compass Hard Iron Offset Auto-Solver  

 2) Accelerometer Yaw Pitch & Roll Calculator  

 3) 360Â° Compass Tilt Compensation


The auto solver is a Gaussian elimination solver requiring at least 6 datasets, the code included will filter and capture 8 datasets around the sphere and compute the best solution possible. Tilt compensation is not angle limited, it will compensate in any orientation.


The library, a more complete description and a 3D scatter plot video are here:  

<http://n0m1.com/2012/02/27/6dof-arduino-compass-accelerometer/>


Michael

## Compilers

- WinAVR (GNU GCC)

## Tags

- General Lib.functions
- WinAVR (GNU GCC)
