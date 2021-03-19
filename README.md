# canvas-drawer

Implements a simple drawing api

TODO: Add a representative image for your project here
![fin](/images/final.png)

## How to build

*Windows*

Open git bash to the directory containing this repository.

```
canvas-drawer $ mkdir build
canvas-drawer $ cd build
canvas-drawer/build $ cmake -G "Visual Studio 16 2019" ..
canvas-drawer/build $ start Draw-2D.sln
```

Your solution file should contain two projects: `pixmap_art` and `pixmap_test`.
To run from the git bash command shell, 

```
canvas-drawer/build $ ../bin/Debug/draw_test
canvas-drawer/build $ ../bin/Debug/draw_art
```

*macOS*

Open terminal to the directory containing this repository.

```
canvas-drawer $ mkdir build
canvas-drawer $ cd build
canvas-drawer/build $ cmake ..
canvas-drawer/build $ make
```

To run each program from build, you would type

```
canvas-drawer/build $ ../bin/draw_test
canvas-drawer/build $ ../bin/draw_art
```

## Supported primitives

TODO: Document the custom primitives supported by your canvas class

DrawCircle:

Takes two ints, which are dictionary keys for the center and the a point on the perimeter of the circle.
Draws a circle around the center with radius equal to the distance between the two points.
Color is interpolated based on the percentage of total area is in a circle with the current radius.
 
![c](/images/circles.png)


DrawAdjecentTriangles:

Takes any number of vertices >= 3.
Draws a triangle between every triplet if vertices (i, i + 1, i + 2).
Ends when i + 2 is great than the number of vertices.

![at](/images/atriangles.png)

DrawUndefined:

Takes an int corrosponding to the number of vertices in the shape.
If the shape has no vertices, it does nothing.
If it has 1, it draws a point.
If it has 2, it draws a line.
If it has 3 or more, it draws triangles between every set of two adjacent vertices and the first vertex with the fill and outline of the first vertex.
The outline is only guarenteed work properly for convex shapes.

![u](/images/undefined.png)

Outlines:
setBorder() takes r,b,g values and sets the border color to the given value.
It also toggles borders to on.
border() turns borders off if it's on and on if it's off (default is off).
The default border color is white.
All shapes except lines can have borders and whether they do depends on the value of bordr when the first vertex is defined.

![oc](/images/bcircles.png)
![ot](/images/btriangles.png)

Fill:
fill() toggles fill, which defaults to true.
This works for all shapes except lines.

![fc](/images/fcircles.png)
![ft](/images/ftriangles.png)
![fu](/images/fundefined.png)
