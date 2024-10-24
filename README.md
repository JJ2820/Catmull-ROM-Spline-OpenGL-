# Catmull-ROM-Spline-OpenGL-
 Catmull-Rom Spline: The interpolation is based on four points (p0, p1, p2, p3), where the resulting curve goes through p1 and p2.
 This is a basic implementation to visualize the spline. You can expand this by loading control points from a file or adjusting the rendering options.



Basic approach was to render a stroke with triangulation outline in OpenGL but was not able to achive it due to some issue with stroke, so used a polygon trioangulation method but output was weird in fill as it takes coordinates next to given path set and makes it complex in bends, so tried to generates a triangle mesh from a list of points so it can be used to draw thick lines but OpenGL do not support line drawing out of the box.
Tried to stroke the width and fill it directly.

Will update if i can find something new.



## References

atmull-Rom Splines in OpenGL 4 with tessellation shaders - https://github.com/enochtsang/catmull_rom_spine_opengl.git

webGL - https://www.shadertoy.com/view/flKcDw

Polyline2D - https://github.com/CrushedPixel/Polyline2D.git


![image](https://github.com/user-attachments/assets/f0123372-f226-475b-b4e5-b134c885d324)

You will get the following output with coordinates  [(0, 0), (1, 1), (2, 3), (5, 1), (7, 8)]
