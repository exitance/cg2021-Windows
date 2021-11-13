#include "Cricle.h"

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
    GLFWwindow* window = glfwCreateWindow(800, 600, " Draw Cricle ", nullptr, nullptr);
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

    std::cout << "--------        Cricle Drawing        --------" << std::endl;
    std::cout << "\tPress C to clear the screen" << std::endl;
    std::cout << "\tClick to choose the center, and" << std::endl;
    std::cout << "\t\tdecide the raduis of the circle" << std::endl;
    std::cout << "----------------------------------------------" << std::endl;

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
            // draw center
            draw_Point(Cle.center.x, Cle.center.y, yellow); 
        }
        else if (rmod == 1)
        {   // draw
            // draw cricle
            circle_midPoint(Cle.center.x, Cle.center.y, Cle.r, yellow);
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
    Cle.center = Point(-1, -1);
    Cle.r = 0;
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

// draw circle point symmetry
// --------------------------
void circle_symmetry_draw(int x0, int y0, int x, int y, Color c)
{
    draw_Point(x+x0, y+y0, c); draw_Point(y+x0, x+y0, c);
    draw_Point(-x+x0, y+y0, c); draw_Point(y+x0, -x+y0, c);
    draw_Point(x+x0, -y+y0, c); draw_Point(-y+x0, x+y0, c);
    draw_Point(-x+x0, -y+y0, c); draw_Point(-y+x0, -x+y0, c);
}

// draw the Circle by mid-point algorithm
// --------------------------------------
void circle_midPoint(int xc, int yc, int r, Color c)
{
    int x, y, d;
    x = 0, y = r;
    d = 4 - 4 * r;
    circle_symmetry_draw(xc, yc, x, y, c);
    while (x <= y)
    {
        if (d < 0)
            d += 4*(2 * x + 3);
        else
        {
            d += 4*(2 * (x - y) + 5);
            y --;
        }
        x ++;
        circle_symmetry_draw(xc, yc, x, y, c);
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
            {   // draw
                reset();
                Cle.center = Point(x, y);  // center of new cricle
                rmod = 0;   // prepare
            }
            else if (rmod == 0)
            {   // prepare
                Point p(x, y);
                Cle.r = p.distance_to(Cle.center);  // radius of current cricle
                rmod = 1;   // draw
            }
            else if (rmod == -1)
            {   // clear screen
                Cle.center = Point(x, y);  // center of new cricle
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
}