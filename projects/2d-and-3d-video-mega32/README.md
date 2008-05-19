# 2D and 3D video on mega32

Uploaded by admin on 2008-05-19 09:28:00 (rating 0 out of 5)

## Summary

Software video generation on the mega32 uses a large percentage of the available cycles, but it is still possible to write video games and data displays. The basic video code generates RS170 (NTSC monochrome) video. There is support for text (in two sizes) and line drawing. 


Four application examples show how to generate smooth animation and how to generate a line drawing perspective view of 3D objects using fast fixed point arithmetic.  

(1) 2D animation of a moving point with reflections at the boundaries.  

(2)A point falling under the influence of gravity with the differential equations approximated using fixed point arithmetic.  

(3) Perspective view of a 3D object with the computer graphic camera on the z-axis looking at the origin. The view-to-screen transformation is calculated in fixed point for speed.  

(4) Orthographic projection of 3D objects is defined by a list of (x,y,z) coordinates and a list of connections between pairs of the coordinates. The connection list defines the edges to be drawn. The view-to-screen transformation is faster than in (3) because there is no perspective divide operation.

## Links

- [Video Generaton on mega32](http://instruct1.cit.cornell.edu/courses/ee476/video/index.html)

## Compilers

- CodeVisionAVR-Standard

## Tags

- Complete code with HW
- CodeVisionAVR-Standard
