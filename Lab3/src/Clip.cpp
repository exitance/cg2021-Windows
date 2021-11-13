#include "Clip.h"

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(800, 600, " Clip Polygon ", nullptr, nullptr);
    if(!window)
    {
        std::cout << "Failed to creat GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetKeyCallback(window, key_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    std::cout << "--------       Polygon Clipping      --------" << std::endl;
    std::cout << "\tPress C to clear the screen" << std::endl;
    std::cout << "\tClick to choose points of the polygon" << std::endl;
    std::cout << "\tPress K to clip the polygon" << std::endl; 
    std::cout << "---------------------------------------------" << std::endl;

    reset();

    while (!glfwWindowShouldClose(window))
    {
        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        draw_corp_coundary(red);

        if (rmod >= 0)
        {   // not cls
            // draw vertices
            for (int i = 0; i < vertices.size(); i ++)
                draw(vertices[i].x, vertices[i].y, yellow, 5);
            // draw edges
            for (int i = 0; i < edges.size(); i ++)
                draw_line(edges[i].vertex[0].x, edges[i].vertex[0].y, edges[i].vertex[1].x, edges[i].vertex[1].y, yellow);
            
            if (rmod == 1)
            {   //clip
                // clip the polygon and show it
                PolygonClip pc(vertices);
                // pc.clipping(white);
                pc.LB_clipping(bule);
            }
        }
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

// reset data, rmod to cls 
// -----------------------
void reset()
{
    rmod = -1;
    vertices.clear();
    edges.clear();
}

// draw a single pixel
// -------------------
void draw(int x, int y, Color c, int sz)
{
    // transform the coordinates: (sw/2, sh/2) => (0, 0)
    // flip the y coordinate: (x, y) => (x, -y)
    // normalization the coordinates: [-1, 1]
    double xpos = (x - sw/2.0) / sw * 2.0;
    double ypos = (sh/2.0 - y) / sh * 2.0;
    glColor3f(c.r, c.g, c.b);
    glPointSize(sz);
    glBegin(GL_POINTS);
    glVertex2f(xpos, ypos);
    glEnd();
}

// draw a line
// -----------
void draw_line(int x0, int y0, int x1, int y1, Color c)
{
    double x0pos = (x0 - sw/2.0) / sw * 2.0;
    double x1pos = (x1 - sw/2.0) / sw * 2.0;
    double y0pos = (sh/2.0 - y0) / sh * 2.0;
    double y1pos = (sh/2.0 - y1) / sh * 2.0;
    glColor3f(c.r, c.g, c.b);
    glBegin(GL_LINES);
    glVertex2f(x0pos, y0pos);
    glVertex2f(x1pos, y1pos);
    glEnd();
}

// draw corp boundary
// ------------------
void draw_corp_coundary(Color c)
{
    draw_line(xl, yb, xl, yt, c);
    draw_line(xr, yb, xr, yt, c);
    draw_line(xl, yb, xr, yb, c);
    draw_line(xl, yt, xr, yt, c);
}

// update parameter for each Crop boundaries
// -----------------------------------------
bool Line::clip_T(float p, float q, float &u_in, float &u_out)
{
    if (p<0)
    {
        float r = q/p;
        if (r > u_out) return false;
        if (r > u_in) u_in = r;
    }
    else if (p>0)
    {
        float r = q/p;
        if (r < u_in) return false;
        if (r < u_out) u_out = r;
    }
    else return q>=0;
    return true;
}

// Liang-Barskey clipping algorithm
// --------------------------------
Line Line::Liang_Barskey()
{
    Line out(Point(-1, -1),Point(-1, -1));

    float dx = vertex[1].x - vertex[0].x;
    float dy = vertex[1].y - vertex[0].y;
    float t_in = 0, t_out = 1;
    if (clip_T(-dx, vertex[0].x-xl, t_in, t_out))
        if (clip_T(dx, xr-vertex[0].x, t_in, t_out))    
            if (clip_T(-dy, vertex[0].y-yb, t_in, t_out))
                if (clip_T(dy, yt-vertex[0].y, t_in, t_out))
                {
                    Point v_in(vertex[0].x + t_in * dx, vertex[0].y + t_in * dy);
                    Point v_out(vertex[0].x + t_out * dx, vertex[0].y + t_out * dy);
                    return Line(v_in, v_out);
                }

    return out;
}

// init the clip boundary
// ----------------------
void PolygonClip::init_bound()
{
    Point lt(xl, yt); Point lb(xl, yb);
    Point rt(xr, yt); Point rb(xr, yb);
    bound.push_back(Line(lb, lt));
    bound.push_back(Line(lt, rt));
    bound.push_back(Line(rt, rb));
    bound.push_back(Line(rb, lb));
}

// get the relation between the k-th clip boundary and given edge
// --------------------------------------------------------------
int PolygonClip::relation(int k, Line edge)
{
    Point va(edge.vertex[0].x - bound[k].vertex[0].x, edge.vertex[0].y - bound[k].vertex[0].y);
    Point vb(edge.vertex[1].x - bound[k].vertex[0].x, edge.vertex[1].y - bound[k].vertex[0].y);
    Point vk(bound[k].vertex[1].x - bound[k].vertex[0].x, bound[k].vertex[1].y - bound[k].vertex[0].y);
    int ca = vk.cross_product(va), cb = vk.cross_product(vb);
    if (ca<=0 && cb<=0) return 1;
    else if (ca>0 && cb>0) return 2;
    else if (ca<=0 && cb>0) return 3;
    else return 4;
}

// get the k-th clip boundary and given edge's intersection 
// --------------------------------------------------------
Point PolygonClip::intersection(int k, Line edge)
{
    Point vab(edge.vertex[1].x-edge.vertex[0].x, edge.vertex[1].y-edge.vertex[0].y);
    Point vkt(bound[k].vertex[1].x-bound[k].vertex[0].x, bound[k].vertex[1].y-bound[k].vertex[0].y);
    Point vka(edge.vertex[0].x-bound[k].vertex[0].x, edge.vertex[0].y-bound[k].vertex[0].y);
    float t = (float)vka.cross_product(vab) / (vkt.cross_product(vab));
    Point ret;
    ret.x = int(bound[k].vertex[0].x + t*vkt.x + 0.5);
    ret.y = int(bound[k].vertex[0].y + t*vkt.y + 0.5);
    return ret;
}

// update the points of polygon using relation between the k-th clip boundary and given edge
// -----------------------------------------------------------------------------------------
void PolygonClip::update_points(std::vector<Point> &tmp, int k, Line edge, Color c)
{
    int rela = relation(k, edge);
    if (rela == 1) tmp.push_back(edge.vertex[1]);
    else if (rela==3 || rela==4)
    {
        Point inter = intersection(k, edge);
        draw(inter.x, inter.y, c, 5);
        if (rela == 3) tmp.push_back(inter);
        else
        {
            tmp.push_back(inter);
            tmp.push_back(edge.vertex[1]);
        }
    }
}

// Clip the Polygon
// ----------------
void PolygonClip::clipping(Color c)
{
    for (int i = 0; i < bound.size(); i ++)
    {
        std::vector<Point> tmp; tmp.clear();
        for (int j = 0; j < vertices.size(); j ++)
        {
            int k = j==vertices.size()-1?0:j+1;
            Line edge(vertices[j], vertices[k]);
            update_points(tmp, i, edge, c);
        }
        vertices = tmp;
    }
    
    for (int i = 0; i < vertices.size(); i ++)
    {
        int j = i==vertices.size()-1?0:i+1;
        edges.push_back(Line(vertices[i], vertices[j]));
        draw_line(edges[edges.size()-1].vertex[0].x, edges[edges.size()-1].vertex[0].y, edges[edges.size()-1].vertex[1].x, edges[edges.size()-1].vertex[1].y, c);
    }
}

// Clip the Polygon using Liang-Barskey algorithm
// ----------------------------------------------
void PolygonClip::LB_clipping(Color c)
{
    std::vector<Point> tmp;
    for (int i = 0; i < vertices.size(); i ++)
    {
        int j = i==vertices.size()-1?0:i+1;
        Line orig_line(vertices[i], vertices[j]);
        Line clip_line = orig_line.Liang_Barskey();
        if (clip_line.vertex[0].valid() && clip_line.vertex[1].valid())
        {
            bool fst = false;
            if ( tmp.empty() || tmp[tmp.size()-1] != clip_line.vertex[0] )
            {
                tmp.push_back(clip_line.vertex[0]);
                fst = true;
            }
            if ( tmp.empty() || tmp[tmp.size()-1] != clip_line.vertex[1] ) 
                if (!fst || (fst && clip_line.vertex[1] != clip_line.vertex[0]) )
                    tmp.push_back(clip_line.vertex[1]);
                
        }
    }
    vertices = tmp;

    for (int i = 0; i < vertices.size(); i ++)
    {
        int j = i==vertices.size()-1?0:i+1;
        edges.push_back(Line(vertices[i], vertices[j]));
        draw_line(edges[edges.size()-1].vertex[0].x, edges[edges.size()-1].vertex[0].y, edges[edges.size()-1].vertex[1].x, edges[edges.size()-1].vertex[1].y, c);
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
    sw = width;
    sh = height;
}

// glfw: whenever the mouse pressed, this callback is called
// -------------------------------------------------------
void mouse_button_callback(GLFWwindow *window, GLint button, GLint action, GLint mods)
{
    if (action == GLFW_PRESS)
    {
        if (button == GLFW_MOUSE_BUTTON_LEFT)
        {
            GLdouble xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            int x = (int)xpos;
            int y = (int)ypos;

            if (rmod == 1)
            {   // clip
                reset();
                rmod = 0;   // prepare
            }
            else if (rmod == 0)
            {   // prepare

            }
            else if (rmod == -1)
            {   // cls
                rmod = 0;   // prepare
            }

            // put new vertex
            Point p(x, y);
            vertices.push_back(p);
            // test if can put new edge
            if (vertices.size() > 1)
            {   // make new edge for polygon
                Line nedge(vertices[vertices.size()-1], vertices[vertices.size()-2]);
                edges.push_back(nedge);
            }

        }
    }
}

// glfw: whenever the key pressed, this callback is called
// -------------------------------------------------------
void key_callback(GLFWwindow *window, GLint key, GLint scancode, GLint action, GLint mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
    else if (key == GLFW_KEY_C && action == GLFW_PRESS)
    {
        reset();
    }
    if (key == GLFW_KEY_K && action == GLFW_PRESS)
    {
        if (rmod == 1)
        {
            
        }
        else
        {
            if (vertices.size() > 2)
            {
                // construct last edge of polygon
                Line ledge(vertices[0], vertices[vertices.size()-1]);
                edges.push_back(ledge);

                rmod = 1; // clip
            }
            else std::cout << "ERROR::POLYGON::INPUT: The number of vertices for a polygon should be at least 3" << std::endl;
        }
    }    
}