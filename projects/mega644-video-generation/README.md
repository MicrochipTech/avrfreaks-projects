# Mega644 video generation

Uploaded by admin on 2009-01-16 17:47:00 (rating 0 out of 5)

## Summary

Software video generation on the mega644 supports 192x150 or 144x200 pixel resolution in RS-170 (NTSC monochrome) format. There is support for text (in two sizes) and line drawing. The video code was written by Shane Pryor.


Two application examples show how to generate smooth animation and how to generate a line drawing perspective view of 3D objects using fast fixed point arithmetic.  

(1)A point falling under the influence of gravity with the differential equations approximated using fixed point arithmetic.  

(2) Orthographic view of a 3D object with the computer graphic camera on the z-axis looking at the origin. The view-to-screen transformation is calculated in fixed point for speed. 3D objects are defined by a list of (x,y,z) coordinates and a list of edge-connections between pairs of the coordinates. The connection list defines the edges to be drawn. 


More information at:  

[http://instruct1.cit.cornell.edu...](http://instruct1.cit.cornell.edu/courses/ee476/FinalProjects/)  

and  

[http://instruct1.cit.cornell.edu...](http://instruct1.cit.cornell.edu/courses/ee476/)

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
