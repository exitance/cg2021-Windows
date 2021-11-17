#ifndef SCAN_H
#define SCAN_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <cstdio>
#include <vector>
#include <list>
#include <algorithm>
#include <sstream>

#include "Exceptions.h"

#define SCR_WIDTH 800
#define SCR_HEIGHT 600

typedef long long ll;

int sw = SCR_WIDTH;     // width of screen
int sh = SCR_HEIGHT;    // height of screen

// Point
struct Point
{
    int x, y;

    Point(){ }
    Point(int _x, int _y):x(_x), y(_y) { }

    void operator=(const Point &b)
    {
        x = b.x;
        y = b.y;
    }
    bool operator==(const Point &b) const
    {
        if (x == b.x && y == b.y) return true;
        return false;
    }
    Point operator-(const Point &b) const
    {
        Point c;
        c.x = this->x - b.x;
        c.y = this->y - b.y;
        return c;
    }

    ll cross_product(const Point &b) const
    {
        // use long long to prevent overflow error
        ll x0 = x, y0 = y;
        ll x1 = b.x, y1 = b.y;
        return x0 * y1 - y0 * x1;
    }
    bool valid() const
    {
        // if it is inside the screen
        if (x >= 0 && y >= 0 && x <= sw && y <= sh) return true;
        return false;
    }
};

// Line
struct Line
{
    Point vertex[2];

    Line() { }
    Line(Point _ver[2])
    {
        vertex[0] = _ver[0];
        vertex[1] = _ver[1];
    }
    Line(const Point &v0, const Point &v1)
    {
        vertex[0] = v0; vertex[1] = v1;
    }    

    // test if end point of the line
    bool end_point(Point b) const
    {
        if (b == vertex[0] || b == vertex[1]) return true;
        return false;
    }
    bool end_point(double x, double y) const
    {
        if (x == vertex[0].x && y == vertex[0].y) return true;
        if (x == vertex[1].x && y == vertex[1].y) return true;
        return false;
    }
    
    // Get *this and line's intersection
    Point intersection(Line line) const;
};

// data structure for polygon scan-converting algorithm: Active Edge
struct ActEdge
{
    int ymax, ymin;
    // deltaX is a constant, while interX is a variable
    double deltaX, interX;

    // default constructor
    ActEdge() { }
    // construct a active edge from a line
    ActEdge(Line line);
    // construct a point-like active edge from given interX and y ( horizental line : a * x + b * y = c && a = 0 Convention deltaX = 0)
    ActEdge(int x, int y)
    {   
        interX = x; deltaX = 0;
        ymin = ymax = y;
    }
};

// RGB Color
struct Color
{
    double r, g, b;

    Color(double _r, double _g, double _b): r(_r), g(_g), b(_b) { }
}red(1, 0, 0), green(0, 1, 0), bule(0, 0, 1), yellow(1, 1, 0);

// simple polygon and scan-converting algorithm for it
class SimplePolygon
{
public:
    // construct a simple polygon from given lines
    SimplePolygon(std::vector<Line> lines);

    // scan-converting algorithm
    void scan_convert(Color c);

private:
    std::vector<ActEdge> edges;
    int ymin, ymax; 
};

int rmod;   // render mode

// data for polygon
std::vector<Point> vertices;
std::vector<Line> edges;

// reset data, rmod to cls
void reset();
// draw a single pixel
void draw(int x, int y, Color c, int sz);
// draw a line
void draw_line(int x0, int y0, int x1, int y1, Color c);
// self-intersect test of polygon
bool check_self_intersect(std::vector<Line> lines);
// glfw callback functions
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_button_callback(GLFWwindow *window, GLint button, GLint action, GLint mods);
void key_callback(GLFWwindow *window, GLint key, GLint scancode, GLint action, GLint mods);

#endif