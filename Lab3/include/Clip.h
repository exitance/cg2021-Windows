#ifndef CLIP_H
#define CLIP_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <cstdio>
#include <vector>
#include <algorithm>

#define SCR_WIDTH 800
#define SCR_HEIGHT 600
#define CLIP_WIDTH 600
#define CLIP_HEIGHT 400
#define CLIP_XL ((SCR_WIDTH-CLIP_WIDTH)/2)
#define CLIP_XR CLIP_XL + CLIP_WIDTH
#define CLIP_YB ((SCR_HEIGHT-CLIP_HEIGHT)/2)
#define CLIP_YT CLIP_YB + CLIP_HEIGHT

int sw = SCR_WIDTH;     // width of screen
int sh = SCR_HEIGHT;    // height of screen
int xl = CLIP_XL;       // left x of Crop box
int xr = CLIP_XR;       // right x of Crop box
int yb = CLIP_YB;       // bottom y of Crop box
int yt = CLIP_YT;       // top y of Crop box

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

    int cross_product(const Point &b)
    {
        return x*b.y - y*b.x;
    }
     bool valid()
    {
        if (x >= 0 && y >= 0) return true;
        return false;
    }
};

// Line with L-B clipping alogrithm
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
        vertex[0] = v0;
        vertex[1] = v1;
    } 

    // update parameter for each Crop boundaries
    bool clip_T(float p, float q, float &u_in, float &u_out);
    // Liang-Barskey clipping algorithm
    Line Liang_Barskey();
};

// RGB Color
struct Color
{
    double r, g, b;

    Color(double _r, double _g, double _b): r(_r), g(_g), b(_b) { }
}red(1, 0, 0), green(0, 1, 0), bule(0, 0, 1), yellow(1, 1, 0), white(1, 1, 1);

// clip polygon method
class PolygonClip
{
public:
    // init the clip boundary
    void init_bound();
    
    // Constructor without input polygon
    PolygonClip() 
    {
        init_bound();
    }
    // Constructor with input polygon
    PolygonClip(std::vector<Point> points)
    {
        vertices = points;
        init_bound();
    }

    // get the relation between the k-th clip boundary and given edge
    int relation(int k, Line edge);
    // get the k-th clip boundary and given edge's intersection 
    Point intersection(int k, Line edge);
    // update the points of polygon using relation between the k-th clip boundary and given edge
    void update_points(std::vector<Point> &tmp, int k, Line edge, Color c);
    // Clip the Polygon
    void clipping(Color c);

    // Clip the Polygon using Liang-Barskey algorithm
    void LB_clipping(Color c);

private:
    std::vector<Point> vertices;
    std::vector<Line> bound;
    std::vector<Line> edges;
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
// draw corp boundary
void draw_corp_coundary(Color c);
// glfw callback functions
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_button_callback(GLFWwindow *window, GLint button, GLint action, GLint mods);
void key_callback(GLFWwindow *window, GLint key, GLint scancode, GLint action, GLint mods);

#endif