#define _USE_MATH_DEFINES

#include <iostream>
#include <map>
#include "canvas.h"
#include <math.h>
using namespace std;
using namespace agl;

int main(int argc, char** argv)
{
	canvas drawer(1100, 1100);
	// your code here

	drawer.background(255, 255, 255);

	int perimX = 1050;
	int perimY = 550;

	for (int i = 5; i > 0; i--) {
		float radis = sqrt(pow(perimX - 550, 2) + pow(perimY - 550, 2));
		
		drawer.begin(CIRCLES);
		drawer.fill();
		drawer.setBorder(255, 0, 51 * i);
		drawer.vertex(550, 550);
		drawer.vertex(perimX, perimY);
		drawer.end();
		
		drawer.begin(UNDEFINED);
		drawer.fill();
		drawer.border();
		drawer.color(51 * (5 - i), 0, 255);
		float baseAngle = 2 * M_PI / (4 + i);
		for (int j = 0; j < 4 + i; j++) {
			float angle = baseAngle * j;
			int x = cos(angle + M_PI / (i + 1)) * radis + 550;
			int y = sin(angle + M_PI / (i + 1)) * radis + 550;
			drawer.vertex(x, y);
		}
		drawer.end();

		drawer.begin(LINES);
		drawer.color(0, 51 * (5- i), 255);
		for (int j = 0; j < 4 + i; j++) {
			float angle = baseAngle * j;
			int x = cos(angle + M_PI / (i + 1)) * radis + 550;
			int y = sin(angle + M_PI / (i + 1)) * radis + 550;
			drawer.vertex(x, y);

			float otherangle = baseAngle * (j + 2);
			int otherx = cos(otherangle + M_PI / (i + 1)) * radis + 550;
			int othery = sin(otherangle + M_PI / (i + 1)) * radis + 550;
			drawer.vertex(otherx, othery);

			if (j == 0) {
				perimX = (x + otherx) / 2;
				perimY = (y + othery) / 2;
			}
		}
		drawer.end();

		
	}
	
	
	drawer.save("test.png");
	
}

