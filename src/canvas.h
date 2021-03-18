#ifndef canvas_H_
#define canvas_H_

#include <string>
#include <vector>
#include <list>
#include <map>

#include "ppm_image.h"

namespace agl
{
   enum PrimitiveType {UNDEFINED, LINES, TRIANGLES, CIRCLES, ADJACENTTRIANGLES};
   class canvas
   {
   public:
      canvas(int w, int h);
      virtual ~canvas();

      // Save to file
      void save(const std::string& filename);

      // Draw primitives with a given type (either LINES or TRIANGLES)
      // For example, the following draws a red line followed by a green line
      // begin(LINES);
      //    color(255,0,0);
      //    vertex(0,0);
      //    vertex(100,0);
      //    color(0,255,0);
      //    vertex(0, 0);
      //    vertex(0,100);
      // end();
      void begin(PrimitiveType type);
      void end();

      // Specifiy a vertex at raster position (x,y)
      // x corresponds to the column; y to the row
      void vertex(int x, int y);

      // Specify a color. Color components are in range [0,255]
      void color(unsigned char r, unsigned char g, unsigned char b);

      // Fill the canvas with the given background color
      void background(unsigned char r, unsigned char g, unsigned char b);

      // Turns border on and sets a color for it
      // Border color defaults to white
      void setBorder(unsigned char r, unsigned char g, unsigned char b);

      // Toggles fill and border for shapes that aren't lines
      // Deciding whether to fill is based on the first vertex
      void fill();
      void border();


   private:
      ppm_image _canvas;
      bool running = false;
      ppm_pixel curColor{0};
      int shapeV;
      PrimitiveType curtype;
      bool bordr = false;
      bool fillCond = true;
      ppm_pixel borderColor{ 255, 255, 255 };

      std::map<int, std::vector<int>> vertices;
      std::map<int, ppm_pixel> colors;
      // where to fill or outline shape
      std::map<int, std::pair<bool, bool>> fbsit;

      int clamp(int, int, int);

      void drawLine(int, int);
      void drawLineX(int, int);
      void drawLineY(int, int);

      void drawTriangle(int, int, int);
      float relPosFromLine(std::vector<int>, std::vector<int>, std::vector<int>);

      // takes two values, the first is the center and the second is on the perimeter
      void drawCircle(int, int);

      // takes an int corrosponding to the number of vertices in the shape
      // draws triangles between every set of two adjacent vertices and the first vertex
      // with the fill, and outline of the first vertex
      // only works for convex shapes
      // if only 2 vertices, draws a line
      // if only 1, a point
      // if none, do nothing
      void drawUndefined(int);
   };
}

#endif


