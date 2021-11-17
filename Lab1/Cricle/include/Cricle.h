#ifndef CRICLE_H
#define CRICLE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <cstdio>
#include <cmath>
#include <algorithm>

#define SCR_WIDTH 800
#define SCR_HEIGHT 600
#define PIXEL_SIZE 10 

int sw = SCR_WIDTH;     // width of screen
int sh = SCR_HEIGHT;    // height of screen
int ps = PIXEL_SIZE;    // diameter of simulated pixel

// simulate pixel
struct Point
{
    int x, y;

    Point(){ }
    Point(int _x, int _y)
    {
        x = _x / ps;
        y = _y / ps;
    }

    void operator=(const Point &b)
    {
        x = b.x;
        y = b.y;
    }

    int distance_to(const Point &b)
    {   // calculate L2 distance
        return (int)sqrt(pow(b.x - x, 2) + pow(b.y - y, 2));
    }
};

// Cricle shape
struct Cricle
{
    Point center;
    int r;
    Cricle() {}
    Cricle(const Point &_c, int _r)
    {
        center = _c;
        r = _r;
    }
}Cle;

// RGB Color
struct Color
{
    double r, g, b;

    Color(double _r, double _g, double _b): r(_r), g(_g), b(_b) { }
}red(1, 0, 0), green(0, 1, 0), bule(0, 0, 1), yellow(1, 1, 0);

int rmod;   // render mode

// reset data, rmod to cls
void reset();
// draw a single pixel
void draw(int x, int y, Color c, int sz);
// draw a simulated pixel
void draw_Point(int x, int y, Color c);
// draw gird
void draw_grid(Color c);
// draw (x, y) and its 7 symmetry points with translation circle center form (0, 0) to (x0, y0)
void circle_symmetry_draw(int x0, int y0, int x, int y, Color c);
// draw the Circle with center (xc, yc) and radius r by mid-point algorithm
void circle_midPoint(int xc, int yc, int r, Color c);
// glfw callback functions
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_button_callback(GLFWwindow *window, GLint button, GLint action, GLint mods);
void key_callback(GLFWwindow *window, GLint key, GLint scancode, GLint action, GLint mods);

#endif