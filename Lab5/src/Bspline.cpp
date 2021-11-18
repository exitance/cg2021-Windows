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
    std::cout << "\t[NOTE]: The default degree of B-spline curve is 4" << std::endl; 
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
    degree = 4;
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

// construct B-spline curve using given control points and degree
// --------------------------------------------------------------
Bspline::Bspline(std::vector<Point> cp, int deg)
{
    control_points = cp;
    degree = deg;
    // Quasi-uniform
    int n = control_points.size() - 1;
    int cnt = n + 1 - degree;
    // 0 and 1 Repeat k times, where k = degree + 1 is the order of the B-spline
    for (int i = 0; i < degree + 1; i ++) knots.push_back(0);
    for (int i = degree + 1; i <= n; i ++) knots.push_back(knots[i-1] + 1.0/cnt);
    for (int i = n + 1; i < n + degree + 2; i ++) knots.push_back(1);
}

// recursively compute the point on B-spline curve with parameter t
// ----------------------------------------------------------------
Point Bspline::recursive_deBoor(double t)
{
    // t in [t_k, t_{k+1}]
    int k = lower_bound(knots, t);

    // p(t) = \sum_{i=0}^{n} ( p_{i} * N_{i, r}(t) ) = \sum_{i=k-degree}^{k} ( p_{i} * N_{i, r}(t) )
    //      = \sum_{i=k-degree}^{k} ( [\frac{t-t_{i}}{t_{i+degree}-t_{i}} * p_{i} + \frac{t_{i+defree}-t}{t_{i+degree}-t_{i}} * p_{i-1}] * N_{i, r-1}(t) )
    // p^[r]_{i}(t) = \frac{t-t_{i}}{t_{i+degree+1-r}-t_{i}} * p^[r-1]_{i} + \frac{t_{i+degree+1-r}-t}{t_{i+degree+1-r}-t_{i}} * p^[r-1]_{i-1}
    // p(t) = p^[degree]_{j}(t)
    // r = 0: p(t)
    std::vector<Point> p;
    // shift - k + degree
    for (int i = k - degree; i <= k + 1; i ++) p.push_back(control_points[i]);

    for (int r = 1; r <= degree; r ++)
    {
        // here i' = i - k + d in original equation
        for (int i = degree; i >= r; i --)
        {
            // here use origin i = i' + k - d
            double alpha = (t - knots[i + k - degree]) / (knots[i + k + 1 - r] - knots[i + k - degree]);
            p[i] = p[i] * alpha + p[i-1] * (1.0 -alpha);
        }
    }

    return p[degree];
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