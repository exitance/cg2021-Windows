#include "Bezier.h"

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
    GLFWwindow* window = glfwCreateWindow(800, 600, " Bezier Curve ", nullptr, nullptr);
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

    std::cout << "-----------------------       Bezier Curve     -----------------------" << std::endl;
    std::cout << "\tPress C to clear the screen" << std::endl;
    std::cout << "\tClick to choose points of the control polygon for Bezier curve" << std::endl;
    std::cout << "----------------------------------------------------------------------" << std::endl;

    reset();

    while (!glfwWindowShouldClose(window))
    {
        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        if (rmod == 0)
        {   // not cls
            // draw control points
            if (control_points.size() > 0)
                for (int i = 0; i < control_points.size(); i ++)
                    draw(control_points[i].x, control_points[i].y, yellow, 5);
            
            if (control_points.size() > 1)
            {
                // draw edges of control polygon
                for (int i = 0; i < control_points.size()-1; i ++)
                    draw_line(control_points[i].x, control_points[i].y, control_points[i+1].x, control_points[i+1].y, yellow);
                
                // draw Bezier curve
                bezier(control_points, blue);
            }
        }

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}

// reset data, rmod to cls 
// -----------------------
void reset()
{
    rmod = -1;
    control_points.clear();
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

// recursively compute the point on bezier curve of parameter t
// ------------------------------------------------------------
Point recursize_bezier(const std::vector<Point> &control_points, double t)
{
    std::vector<Point> split_points = control_points;
    while(split_points.size() > 1)
    {
        for (int i = 0; i < split_points.size()-1; i ++)
            split_points[i] = (split_points[i] * (1-t) + split_points[i+1] * t);
        split_points.pop_back();
    }
    return split_points[0];
}

// draw bezier curve
// -----------------
void bezier(const std::vector<Point> &control_points, Color c)
{
    for (double t = 0.0; t <= 1.0; t += 0.001)
    {
        Point point = recursize_bezier(control_points, t);
        draw(point.x, point.y, c, 1);
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

            if (rmod == 0)
            {   // draw

            }
            else if (rmod == -1)
            {   // cls
                rmod = 0;   // draw
            }

            // put new point
            Point p(x, y);
            control_points.push_back(p);
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