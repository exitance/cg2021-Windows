#ifndef BSPLINE_H
#define BSPLINE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <cstdio>
#include <cmath>
#include <vector>
#include <algorithm>

#define SCR_WIDTH 800
#define SCR_HEIGHT 600
#define CONTROL_POINT_SIZE 20

int sw = SCR_WIDTH;             // screen width
int sh = SCR_HEIGHT;            // screen height
int cps = CONTROL_POINT_SIZE;   // size of control point

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
    bool operator==(const Point &b)
    {
        if (x == b.x && y == b.y) return true;
        return false;
    }
    bool operator!=(const Point &b)
    {
        if (x != b.x || y != b.y) return true;
        return false;
    }
    Point operator*(double v)
    {
        return Point(v*x, v*y);
    }
    Point operator+(const Point &b)
    {
        return Point(x+b.x, y+b.y);
    }

    int cross_product(const Point &b)
    {
        return x*b.y - y*b.x;
    }
};

// RGB Color
struct Color
{
    double r, g, b;

    Color(double _r, double _g, double _b): r(_r), g(_g), b(_b) { }
}red(1, 0, 0), green(0, 1, 0), blue(0, 0, 1), yellow(1, 1, 0), white(1, 1, 1);

// B-spline and deBoor algorithm
class Bspline
{
public:
    // default constructor
    Bspline(){ }
    // construct B-spline curve using given control points and degree
    Bspline(std::vector<Point> cp, int deg); 

    // recursively compute the point on B-spline curve with parameter t
    Point recursive_deBoor(double t);
    // draw B-spline curve using deBoor algorithm
    void deBoor(Color c);

private:
    std::vector<Point> control_points;
    std::vector<double> knots;
    int degree;
};

int rmod;       // render mode

int degree;  // degree of B-spline curve
std::vector<Point> control_points;  // control points for B-spline curve

int index;  // index of current select control point

// reset data, rmod to cls
void reset();
// draw a single pixel
void draw(int x, int y, Color c, int sz);
// draw a line
void draw_line(int x0, int y0, int x1, int y1, Color c);
// get the index of the rightmost element in ascending array a which value is less than or equal to x
int lower_bound(std::vector<double> a, double x);
// check if (x, y) near some control point
int check_control_point(int x, int y);
// modify control point[idx] of B-spline to (x, y)
void modify_control_point(int idx, int x, int y);
// glfw callback functions
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_button_callback(GLFWwindow *window, GLint button, GLint action, GLint mods);
void key_callback(GLFWwindow *window, GLint key, GLint scancode, GLint action, GLint mods);
void cursor_position_callback(GLFWwindow *window, double xpos, double ypos);

#endif