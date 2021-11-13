#include "Bspline.h"

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
    GLFWwindow* window = glfwCreateWindow(800, 600, " B-spline Curve ", nullptr, nullptr);
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
    glfwSetCursorPosCallback(window, cursor_position_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    std::cout << "-------------------------       B-spline Curve      -------------------------" << std::endl;
    std::cout << "\tPress C to clear the window" << std::endl;
    std::cout << "\tClick to choose points of the control polygon for B-spline curve" << std::endl;
    std::cout << "\tDrag existed control point to adjust B-spline curve" << std::endl;
    std::cout << "\tRight-click existed control point to delete the control point" << std::endl;
    std::cout << "\tPress D then enter a integer can change the degree of B-spline curve" << std::endl;
    std::cout << "\t[NOTE]: The default degree of B-spline curve is 3" << std::endl; 
    std::cout << "-----------------------------------------------------------------------------" << std::endl;

    reset();

    while (!glfwWindowShouldClose(window))
    {
        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        if (rmod >= 0)
        {   // not cls
            // draw control points
            if (control_points.size() > 0)
                for (int i = 0; i < control_points.size(); i ++)
                    draw(control_points[i].x, control_points[i].y, yellow, cps);
            
            // draw edges of control polygon
            if (control_points.size() > 1)
            {
                for (int i = 0; i < control_points.size()-1; i ++)
                    draw_line(control_points[i].x, control_points[i].y, control_points[i+1].x, control_points[i+1].y, yellow);
            }

            // draw B-spline curve
            if (control_points.size()-1 >= degree)
            {
                Bspline bp(control_points, degree);
                bp.deBoor(blue);
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
    index = -1;
    degree = 3;
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

// get the index of the rightmost element in ascending array a which value is less than or equal to x
// --------------------------------------------------------------------------------------------------
int lower_bound(std::vector<double> a, double x)
{
    int l = 0, r = a.size()-1;
    while(l < r)
    {
        int mid = (l+r+1) >> 1;
        if (a[mid] <= x) l = mid;
        else if (a[mid] > x) r = mid - 1;
    }
    return l;
}

// check if (x, y) near some control point
// ---------------------------------------
int check_control_point(int x, int y)
{
    for (int i = 0; i < control_points.size(); i ++)
        if (sqrt(pow(x-control_points[i].x, 2) + pow(y-control_points[i].y, 2)) <= cps)
            return i;
    return -1;
}

// modify control point[idx] of B-spline to (x, y)
// -----------------------------------------------
void modify_control_point(int idx, int x, int y)
{
    control_points[idx].x = x;
    control_points[idx].y = y;
}

// recursively compute the point on B-spline curve with parameter t
// ----------------------------------------------------------------
Point Bspline::recursive_deBoor(double t)
{
    int k = lower_bound(knots, t);
    std::vector<Point> d;
    for (int i = k - degree; i <= k + 1; i ++) d.push_back(control_points[i]);

    for (int r = 1; r <= degree; r ++)
    {
        for (int j = degree; j >= r; j --)
        {
            double alpha = (t - knots[j + k - degree]) / (knots[j + 1 + k - r] - knots[j + k - degree]);
            Point tmp;
            tmp.x = (1.0 - alpha) * d[j - 1].x + alpha * d[j].x;
            tmp.y = (1.0 - alpha) * d[j - 1].y + alpha * d[j].y;
            d[j] = tmp;
        }
    }

    return d[degree];
}

// draw B-spline curve using deBoor algorithm
// ------------------------------------------
void Bspline::deBoor(Color c)
{
    for (double t = 0.0; t <= 1.0; t += 0.001)
    {
        Point point = recursive_deBoor(t);
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
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        if (rmod == 0)
        {   // draw
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            int x = (int)xpos;
            int y = (int)ypos;
            index = check_control_point(x, y);
            if (index >= 0) rmod = 1;   // modify
            else
            {   // add new control point
                Point p(x, y);
                control_points.push_back(p);
            }
        }
        else if (rmod == -1)
        {   // cls
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            int x = (int)xpos;
            int y = (int)ypos;
            Point p(x, y);
            control_points.push_back(p);
            rmod = 0;   // draw
        }
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {   // delete control point
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        int x = (int)xpos;
        int y = (int)ypos;
        // if click on a control point
        int idx = check_control_point(x, y);
        // delete it
        if (idx >= 0) control_points.erase(control_points.begin() + idx);
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
    else if (key == GLFW_KEY_D && action == GLFW_PRESS)
    {
        std::cout << "[Modify the Degree of B-spline] Please Enter Degree: ";
        std::cin >> degree;
    }
}

// glfw: whenever the cursor moves, this callback is called
// --------------------------------------------------------
void cursor_position_callback(GLFWwindow *window, double xpos, double ypos)
{
    if (rmod == 1)
    {   // modify
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
            rmod = 0;   // draw
        else
        {
            int x = (int)xpos;
            int y = (int)ypos;
            modify_control_point(index, x, y);
        }
    }
}