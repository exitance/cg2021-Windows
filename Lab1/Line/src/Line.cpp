#include "Line.h"

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
    GLFWwindow* window = glfwCreateWindow(800, 600, " Draw Line ", nullptr, nullptr);
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

    std::cout << "--------              Line Drawing              --------" << std::endl;
    std::cout << "\tPress C to clear the screen " << std::endl;
    std::cout << "\tClick the pixel to choose 2 endpoints of the Line " << std::endl;
    std::cout << "\tPress key to choose algorithm: " << std::endl;
    std::cout << "\t\tD for DDA " << std::endl;
    std::cout << "\t\tB for Bresenham " << std::endl;
    std::cout << "-------------------------------------------------------" << std::endl;

    reset();

    while (!glfwWindowShouldClose(window))
    {
        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        draw_grid(red);

        if (rmod == 0)
        {   // prepare
            // draw 1 point
            draw_Point(Line[0].x, Line[0].y, yellow); 
        }
        else if (rmod == 1)
        {   // ready
            // draw 2 point
            for (int i = 0; i < 2; i ++)
                draw_Point(Line[i].x, Line[i].y, yellow); 
        }
        else if (rmod == 2)
            draw_Line(yellow);

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
    for (int i = 0; i < 2; i ++) Line[i] = Point(-1, -1);
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

// draw simulated pixel
// --------------------
void draw_Point(int x, int y, Color c)
{
    int xmin = x * ps, ymin = y * ps;
    int xmax = xmin + ps, ymax = ymin + ps;
    double x_min = (xmin - sw/2.0) / sw * 2.0;
    double y_min = (sh/2.0 - ymin) / sh * 2.0;
    double x_max = (xmax - sw/2.0) / sw * 2.0;
    double y_max = (sh/2.0 - ymax) / sh * 2.0;
    glColor3f(c.r, c.g, c.b);
    glBegin(GL_TRIANGLES);
    glVertex2f(x_min, y_min);
    glVertex2f(x_min, y_max);
    glVertex2f(x_max, y_min);
    glVertex2f(x_min, y_max);
    glVertex2f(x_max, y_min);
    glVertex2f(x_max, y_max);
    glEnd();
}

// draw gird
// ---------
void draw_grid(Color c)
{
    double y_min = -1.0;
    double y_max = 1.0;
    for (int x = 0; x <= sw; x += ps)
    {
        double xpos = (x - sw/2.0) / sw * 2.0;
        glColor3f(c.r, c.g, c.b);
        glBegin(GL_LINES);
        glVertex2f(xpos, y_min);
        glVertex2f(xpos, y_max);
        glEnd();
    }
    double x_min = -1.0;
    double x_max = 1.0;
    for (int y = 0; y <= sh; y += ps)
    {
        double ypos = (sh/2.0 - y) / sh * 2.0;
        glColor3f(c.r, c.g, c.b);
        glBegin(GL_LINES);
        glVertex2f(x_min, ypos);
        glVertex2f(x_max, ypos);
        glEnd();
    } 
}

// DDA algorithm draw line
// -----------------------------
void DDA(int x0, int y0, int x1, int y1, Color c)
{
    if (x1==x0 && y1==y0) return;
    float dx = x1 - x0, dy = y1 - y0;
    int my = 1; if (y1 < y0) my = -1;
    int mx = 1; if (x1 < x0) mx = -1;
    if ( abs(dy) > abs(dx) )
    {
        float f = dx / dy;
        float x = x0;
        for (int y = y0; y != y1 + my; y += my)
        {
            draw_Point(int(x+0.5), y, c);
            x = x + my * f;
        }
    }
    else
    {
        float k = dy / dx; 
        float y = y0;
        for (int x = x0; x != x1 + mx; x += mx)
        {
            draw_Point(x, int(y+0.5), c);
            y = y + mx * k;
        }
    }
}

// Bresenham algorithm draw line
// -----------------------------
void Bresenham(int x0, int y0, int x1, int y1, Color c)
{
    if (x1==x0 && y1==y0) return;
    int dx = x1 - x0, dy = y1 - y0;
    if ( abs(dy) > abs(dx) )
    {
        if (y1 < y0)
        {
            std::swap(y1, y0);
            std::swap(x1, x0);
        }
        int mx = 1; if (x1 < x0) mx = -1;
        for (int y = y0, x = x0, e = -mx*dy; y <= y1; y ++)
        {
            draw_Point(x, y, c);
            e += 2*dx;
            if ( (dx>0 && e>=0) || (dx<0 && e<=0) )
            {
                x += mx;
                e -= 2*mx*dy;
            }
        }
    }
    else
    {
        if (x1 < x0)
        {
            std::swap(y1, y0);
            std::swap(x1, x0);
        }
        int my = 1; if (y1 < y0) my = -1;
        for (int x = x0, y = y0, e= -my*dx; x <= x1; x ++)
        {
            draw_Point(x, y, c);
            e += 2*dy;
            if ( (dy>0 && e>=0) || (dy<0 && e<=0) )
            {
                y += my;
                e -= 2*my*dx;
            }
        }
    }
}

// draw the Line
// -------------
void draw_Line(Color c)
{
    if (algo == 1)
    {   // DDA algorithm
        DDA(Line[0].x, Line[0].y, Line[1].x, Line[1].y, c);
    }
    else if (algo == 2)
    {   // Bresenham algorithm
        Bresenham(Line[0].x, Line[0].y, Line[1].x, Line[1].y, c);
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

            if (rmod == 2)
            {   // draw
                reset();
                Line[0] = Point(x, y);  // 1st point of Line
                rmod = 0;   // prepare
            }
            else if (rmod == 1)
            {   // ready
                printf("\t[Warning]: Please select a algorithm to draw current Line\n");
            }
            else if (rmod == 0)
            {   // prepare
                Line[1] = Point(x, y);  // 2nd point of Line
                rmod = 1;   // ready
            }
            else if (rmod == -1)
            {   // clear screen
                Line[0] = Point(x, y);  // 1st point of Line
                rmod = 0;   // prepare
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
    if (rmod == 1)
    {   // ready
        if (key == GLFW_KEY_D && action == GLFW_PRESS)
            algo = 1;
        else if (key == GLFW_KEY_B && action == GLFW_PRESS)
            algo = 2;
        rmod = 2;   // draw
    }
}