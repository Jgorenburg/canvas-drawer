#include "canvas.h"
#include <cassert>
#include <list>
#include <map>
#include <vector>

using namespace std;
using namespace agl;

canvas::canvas(int w, int h) : _canvas(w, h)
{
	ppm_image _canvas(w, h);
}

canvas::~canvas()
{
}

void canvas::save(const std::string& filename)
{
   _canvas.save(filename);
}

void canvas::begin(PrimitiveType type)
{
	assert(!running);
	running = true;
	vertices = map<int, vector<int>>();
	colors = map<int, ppm_pixel>();
	curtype = type;
	bordr = false;

	if (type == LINES || type == CIRCLES) {
		shapeV = 2;
	}
	else if (type == TRIANGLES) {
		shapeV = 3;
	}
	else if (type == ADJACENTTRIANGLES) {
		shapeV = 1;
	}
	else if (type == UNDEFINED) {
		shapeV = 0;
	}
	
}

void canvas::end()
{
	assert(running);
	running = false;

	if (shapeV != 0) {
		// check that we have right num of Vs
		assert(vertices.size()%shapeV == 0);
		
		
		int count = 0;
		
		if (curtype == LINES) {
			while (count < vertices.size()) {
				drawLine(count, count + 1);
				count += 2;
			}
		}
		else if (curtype == TRIANGLES) {
			while (count < vertices.size()) {
				drawTriangle(count, count + 1, count + 2);
				count += 3;
			}
		}
		else if (curtype == CIRCLES) {
			while (count < vertices.size()) {
				drawCircle(count, count + 1);
				count += 2;
			}
		}
		// draws triangles for every triplet of adjacent vertices
		else if (curtype == ADJACENTTRIANGLES) {
			assert(vertices.size() >= 3);
			count = 2;
			while (count < vertices.size()) {
				drawTriangle(count - 2, count - 1, count);
				count++;
			}
		}

	}
	else {
		drawUndefined(vertices.size());
	}
	bordr = false;
	fillCond = true;
	borderColor = ppm_pixel{ 255,255,255 };
}

void canvas::vertex(int x, int y)
{
	int clampedX = clamp(x, 0, _canvas.width() - 1);
	int clampedY = clamp(y, 0, _canvas.height() - 1);
	int name = vertices.size();
	vector<int> pos;
	pos.push_back(clampedX);
	pos.push_back(clampedY);
	vertices[name] = pos;
	colors[name] = curColor;
	fbsit[name] = pair<bool, bool>(fillCond, bordr);
}

int canvas::clamp(int val, int min, int max) {
	if (val < min)		{ return min; }
	else if (val > max) { return max; }
	else				{ return val; }
}

void canvas::color(unsigned char r, unsigned char g, unsigned char b)
{
	curColor = ppm_pixel{r, g, b};
}

void canvas::background(unsigned char r, unsigned char g, unsigned char b)
{
	for (int i = 0; i < _canvas.height(); i++) {
		for (int j = 0; j < _canvas.width(); j++) {
			_canvas.set(i, j, ppm_pixel{ r,g,b });
		}
	}
}

void canvas::drawLine(int first, int second) {
	int start = first;
	int end = second;
	// makes sure start has smaller y coord
	if (vertices[first][1] > vertices[second][1]) {
		start = second;
		end = first;
	}

	vector<int> v1 = vertices[start];
	vector<int> v2 = vertices[end];


	float x = v2[0] - v1[0];
	if (x != 0) {
		float slope = (v2[1] - v1[1]) / x;
		if (abs(slope) <= 1) {
			drawLineX(first, second);
		}
		else {
			drawLineY(first, second);
		}
	}
	else {
		// when the slope is undefined
		int ystart = v1[1];
		int yend = v2[1];
		int length = yend - ystart;

		ppm_pixel color1 = colors[start];
		ppm_pixel color2 = colors[end];

		for (int y = ystart; y <= yend; y++) {
			// color is interpolated based on distance from one of the vertices
			float dist = y - ystart;
			float distRatio = dist / max(length, 1);
			int r = color2.r * distRatio + color1.r * (1 - distRatio);
			int g = color2.g * distRatio + color1.g * (1 - distRatio);
			int b = color2.b * distRatio + color1.b * (1 - distRatio);
			_canvas.set(y, v1[0], ppm_pixel{ static_cast<unsigned char>(r),static_cast<unsigned char>(g),static_cast<unsigned char>(b) });
		}
	}
}

void canvas::drawLineX(int first, int second) {

	int start = first;
	int end = second;
	// makes sure start has smaller x coord
	if (vertices[first][0] > vertices[second][0]) {
		start = second;
		end = first;
	}
	vector<int> v1 = vertices[start];
	vector<int> v2 = vertices[end];
	ppm_pixel color1 = colors[start];
	ppm_pixel color2 = colors[end];
	
	int y = v1[1];
	int W = v2[0] - v1[0];
	int H = v2[1] - v1[1];
	int inc = 1;
	if (H < 0) {
		inc = -1;
		H = -H;
	}
	int F = 2*H - W;

	float length = sqrt(pow(v2[0] - v1[0], 2) + pow(v2[1] - v1[1], 2));
	for (int x = v1[0]; x <= v2[0]; x++) {
		// Creating a color gradiant
		float dist = sqrt(pow(x - v1[0], 2) + pow(y - v1[1], 2));
		float distRatio = dist / max(length, 1.0f);
		int r = color2.r * distRatio + color1.r * (1 - distRatio);
		int g = color2.g * distRatio + color1.g * (1 - distRatio);
		int b = color2.b * distRatio + color1.b * (1 - distRatio);
		_canvas.set(y, x, ppm_pixel{static_cast<unsigned char>(r),static_cast<unsigned char>(g),static_cast<unsigned char>(b) });
		if (F > 0) {
			y += inc;
			F += 2 * (H - W);
		}
		else {
			F += 2 * H;
		}
	}

	
}


void canvas::drawLineY(int first, int second) {

	int start = first;
	int end = second;
	// makes sure start has smaller y coord
	if (vertices[first][1] > vertices[second][1]) {
		start = second;
		end = first;
	}
	vector<int> v1 = vertices[start];
	vector<int> v2 = vertices[end];
	ppm_pixel color1 = colors[start];
	ppm_pixel color2 = colors[end];

	int x = v1[0];
	int W = v2[0] - v1[0];
	int H = v2[1] - v1[1];
	int inc = 1;
	if (W < 0) {
		inc = -1;
		W = -W;
	}
	int F = 2 * W - H;

	float length = sqrt(pow(v2[0] - v1[0], 2) + pow(v2[1] - v1[1], 2));

	for (int y = v1[1]; y <= v2[1]; y++) {
		// Creating a color gradiant
		float dist = sqrt(pow(x - v1[0], 2) + pow(y - v1[1], 2));
		float distRatio = dist / max(length, 1.0f);
		int r = color2.r * distRatio + color1.r * (1 - distRatio);
		int g = color2.g * distRatio + color1.g * (1 - distRatio);
		int b = color2.b * distRatio + color1.b * (1 - distRatio);
		_canvas.set(y, x, ppm_pixel{ static_cast<unsigned char>(r),static_cast<unsigned char>(g),static_cast<unsigned char>(b) });
		if (F > 0) {
			x += inc;
			F += 2 * (W - H);
		}
		else {
			F += 2 * W;
		}
	}
}

void canvas::drawTriangle(int first, int second, int third) {
	vector<int> a = vertices[first];
	vector<int> b = vertices[second];
	vector<int> c = vertices[third];
	ppm_pixel colora = colors[first];
	ppm_pixel colorb = colors[second];
	ppm_pixel colorc = colors[third];

	bool shouldFill = fbsit[first].first;
	bool shouldOutline = fbsit[first].second;

	
	int minX = min(min(a[0], b[0]), c[0]);
	int maxX = max(max(a[0], b[0]), c[0]);
	int minY = min(min(a[1], b[1]), c[1]);
	int maxY = max(max(a[1], b[1]), c[1]);

	float f_alpha = relPosFromLine(b, c, a);
	float f_beta = relPosFromLine(a, c, b);
	float f_gamma = relPosFromLine(a, b, c);

	// fills in the triangle
	if (shouldFill) {
		for (int i = minY; i <= maxY; i++) {
			for (int j = minX; j <= maxX; j++) {
				// need (i, j) as vector for relPosFromLine
				vector<int> curPos;
				curPos.push_back(j);
				curPos.push_back(i);
				float alpha = relPosFromLine(b, c, curPos) / f_alpha;
				float beta = relPosFromLine(a, c, curPos) / f_beta;
				float gamma = relPosFromLine(a, b, curPos) / f_gamma;

				if (alpha >= 0 && alpha <= 1 && beta >= 0 && beta <= 1 && gamma >= 0 && gamma <= 1) {
					int r = colora.r * alpha + colorb.r * beta + colorc.r * gamma;
					int g = colora.g * alpha + colorb.g * beta + colorc.g * gamma;
					int b = colora.b * alpha + colorb.b * beta + colorc.b * gamma;

					ppm_pixel rgb{ static_cast<unsigned char>(r),static_cast<unsigned char>(g),static_cast<unsigned char>(b) };
					_canvas.set(i, j, rgb);
				}
			}
		}
	}
	// draws three lines to outline the triangle
	if (shouldOutline) {
		// we add three new vertices to vertices to make sure the color is borderColor
		// the erases ensure that all data structs end up unchanged
		color(borderColor.r, borderColor.g, borderColor.b);
		vertex(a[0], a[1]);
		vertex(b[0], b[1]);
		vertex(c[0], c[1]);
		drawLine(vertices.size() - 3, vertices.size() - 2);
		drawLine(vertices.size() - 2, vertices.size() - 1);
		drawLine(vertices.size() - 3, vertices.size() - 1);
		vertices.erase(vertices.size() - 1);
		vertices.erase(vertices.size() - 1);
		vertices.erase(vertices.size() - 1);
		colors.erase(colors.size() - 1);
		colors.erase(colors.size() - 1);
		colors.erase(colors.size() - 1);
		
	}
}

float canvas::relPosFromLine(vector<int> first, vector<int> second, vector<int> pos) {
	return (second[1] - first[1]) * (pos[0] - first[0]) - (second[0] - first[0]) * (pos[1] - first[1]);
}

void canvas::drawCircle(int c, int p) {
	vector<int> center = vertices[c];
	vector<int> perim = vertices[p];
	ppm_pixel colorc = colors[c];
	ppm_pixel colorp = colors[p];

	bool shouldFill = fbsit[c].first;
	bool shouldOutline = fbsit[c].second;

	// use a^2 + b^2 = c^2 to find the length of the radius
	float radius = sqrt(pow(perim[0] - center[0], 2) + pow(perim[1] - center[1], 2));
	
	// the 4 numbers represent the bounding box of the circle
	int minX = max(0.0f, center[0] - radius);
	int maxX = min(_canvas.width() - 1.0f, center[0] + radius);
	int minY = max(0.0f, center[1] - radius);
	int maxY = min(_canvas.height() - 1.0f, center[1] + radius);
	
	for (int i = minY; i <= maxY; i++) {
		for (int j = minX; j <= maxX; j++) {
			// distRatio is asigned this way to avoid a divide by 0 error
			// if radius == 0, the only colored point should be the center
			float dist = sqrt(pow(i - center[1], 2) + pow(j - center[0], 2));
			float distRatio = dist;
			if (radius != 0) { distRatio = dist / radius; }
			if (distRatio <= 1) {
				// the formula for interpolation is not linear
				// instead it is based on area
				float colorRatio = 1;
				if (radius != 0) { colorRatio = pow(dist, 2) / pow(radius, 2); }
				int r = colorp.r * colorRatio + colorc.r * (1 - colorRatio);
				int g = colorp.g * colorRatio + colorc.g * (1 - colorRatio);
				int b = colorp.b * colorRatio + colorc.b * (1 - colorRatio);
				ppm_pixel rgb{ static_cast<unsigned char>(r),static_cast<unsigned char>(g),static_cast<unsigned char>(b) };
				
				if (shouldFill) {
					_canvas.set(i, j, rgb);
				}
			}
			if (shouldOutline && ceil(radius) == ceil(dist)) {
				_canvas.set(i, j, borderColor);
			}
		}
	}

}



void canvas::setBorder(unsigned char r, unsigned char g, unsigned char b) {
	bordr = true;
	borderColor = ppm_pixel{ r, g, b };
}

void canvas::border() {
	bordr = !bordr;
}

void canvas::fill() {
	fillCond = !fillCond;
}


void canvas::drawUndefined(int numVertices) {
	vector<int> mainV = vertices[0];
	ppm_pixel mainColor = colors[0];

	bool shouldOutline = fbsit[0].second;
	// turns this false to prevent internal outlining
	fbsit[0].second = false;


	if (numVertices >= 3) {
		int count = 2;
		while (count < numVertices) {
			drawTriangle(0, count - 1, count);
			count++;
		}
		// draws lines between all adjacent vertices with the 
		// color of the first vertex
		if (shouldOutline) {
			color(mainColor.r, mainColor.g, mainColor.b);
			vertex(vertices[0][0], vertices[0][1]);
			for (int i = 0; i < numVertices; i++) {
				vertex(vertices[i + 1][0], vertices[i + 1][1]);
				drawLine(i + numVertices, i + numVertices + 1);
			}
		}
	}
	else if (numVertices == 2) {
		drawLine(0, 1);
	}
	else if (numVertices == 1) {
		_canvas.set(mainV[1], mainV[0], mainColor);
	}
}
