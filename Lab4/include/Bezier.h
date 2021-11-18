#ifndef BEZIER_H
#define BEZIER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <cstdio>
#include <vector>
#include <algorithm>

#define SCR_WIDTH 800
#define SCR_HEIGHT 600

int sw = SCR_WIDTH;     // screen width
int sh = SCR_HEIGHT;    // screen height

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
    Point operator*(double v)
    {
        return Point(v*x, v*y);
    }
    Point operator+(const Point &b)
    {
        return Point(x+b.x, y+b.y);
    }
};

// RGB Color
struct Color
{
    double r, g, b;

    Color(double _r, double _g, double _b): r(_r), g(_g), b(_b) { }
}red(1, 0, 0), green(0, 1, 0), blue(0, 0, 1), yellow(1, 1, 0), white(1, 1, 1);

int rmod;   // render mode

std::vector<Point> control_points;  // control points for Bezier curve

// reset data, rmod to cls
void reset();
// draw a single pixel
void draw(int x, int y, Color c, int sz);
// draw a line
void draw_line(int x0, int y0, int x1, int y1, Color c);
// recursively compute the point on bezier curve of parameter t
Point recursize_bezier(const std::vector<Point> &control_points, double t);
// draw bezier curve
void bezier(const std::vector<Point> &control_points, Color c);
// glfw callback functions
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_button_callback(GLFWwindow *window, GLint button, GLint action, GLint mods);
void key_callback(GLFWwindow *window, GLint key, GLint scancode, GLint action, GLint mods);

#endif