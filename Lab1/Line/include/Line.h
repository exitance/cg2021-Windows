#ifndef LINE_H
#define LINE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <cstdio>
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
}Line[2];   // Line

// RGB Color
struct Color
{
    double r, g, b;

    Color(double _r, double _g, double _b): r(_r), g(_g), b(_b) { }
}red(1, 0, 0), green(0, 1, 0), bule(0, 0, 1), yellow(1, 1, 0);

int rmod;   // render mode
int algo;   // algorithm

// reset data, rmod to cls
void reset();
// draw a single pixel
void draw(int x, int y, Color c, int sz);
// draw a simulated pixel
void draw_Point(int x, int y, Color c);
// draw gird
void draw_grid(Color c);
// DDA algorithm draw line
void DDA(int x0, int y0, int x1, int y1, Color c);
// Bresenham algorithm draw line
void Bresenham(int x0, int y0, int x1, int y1, Color c);
// draw the Line
void draw_Line(Color c);
// glfw callback functions
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_button_callback(GLFWwindow *window, GLint button, GLint action, GLint mods);
void key_callback(GLFWwindow *window, GLint key, GLint scancode, GLint action, GLint mods);

#endif