#include "Scan.h"

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
    GLFWwindow* window = glfwCreateWindow(800, 600, " Scan Convert ", nullptr, nullptr);
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

    std::cout << "------------      Scan Converting       ------------" << std::endl;
    std::cout << "\tPress C to clear the screen" << std::endl;
    std::cout << "\tClick to choose points of the simple polygon" << std::endl;
    std::cout << "\tPress D to draw the simple polygon" << std::endl;
    std::cout << "----------------------------------------------------" << std::endl;

    reset();

    while (!glfwWindowShouldClose(window))
    {
        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        if (rmod >= 0)
        {   // not cls
            // draw vertices
            for (int i = 0; i < vertices.size(); i ++)
                draw(vertices[i].x, vertices[i].y, yellow, 5);
            // draw edges
            for (int i = 0; i < edges.size(); i ++)
                draw_line(edges[i].vertex[0].x, edges[i].vertex[0].y, edges[i].vertex[1].x, edges[i].vertex[1].y, yellow);

            if (rmod == 1)
            {   // draw
                // test if simple polygon
                // draw it
                try
                {
                    SimplePolygon sg(edges);
                    sg.scan_convert(yellow);
                }
                catch(EdgeIntersect e)
                {
                    reset();
                    e.outputMsg();
                }
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

// self-intersect test of polygon
// ------------------------------
bool check_self_intersect(std::vector<Line> lines)
{   // simple implementation
    // for every pair of lines
    for (int i = 0; i < lines.size(); i ++)
    {
        for (int j = i+1; j < lines.size(); j ++)
        {   // test if intersection exists
            Point inter = lines[i].intersection(lines[j]);
            if (inter.valid()) return true;
        }
    }
    return false;
}

// Get *this and line's intersection
// -------------------------------
Point Line::intersection(Line line) const
{   // If here is an intersection between the two lines
    // if it is, return the intersection
    // else return Point(-1, -1)

    // Test if *this cross the line
    // that is, vec20 and vec21 are on opposite side of vec23
    Point vec20 = vertex[0] - line.vertex[0];       // vector 2->0
    Point vec21 = vertex[1] - line.vertex[0];       // vector 2->1
    Point vec23 = line.vertex[1] - line.vertex[0];  // vector 2->3
    // using cross product
    ll c1 = vec23.cross_product(vec20), c2 = vec23.cross_product(vec21);
    if (c1 * c2 > 0) return Point(-1, -1);

    // Test if the line cross *this
    // that is, vec02 and vec03 are on opposite side of vec01
    Point vec02 = line.vertex[0] - vertex[0];       // vector 0->2
    Point vec03 = line.vertex[1] - vertex[0];       // vector 0->3
    Point vec01 = vertex[1] - vertex[0];            // vector 0->1
    // using cross product
    ll c3 = vec01.cross_product(vec02), c4 = vec01.cross_product(vec03);
    if (c3 * c4 > 0) return Point(-1, -1);

    // intersection exists, compute
    // S013 / S123 = t / 1-t
    double t = (double)(c1) / ( (double)(c1) - (double)(c2) );
    if (t < 0) t = -t;
    double x = vertex[0].x + t*vec01.x;
    double y = vertex[0].y + t*vec01.y;
    // End point of 2 line
    if (end_point(x, y) && line.end_point(x, y)) return Point(-1, -1);
    // return the intersection
    Point inter((int)(x + 0.5), (int)(y + 0.5));
    return inter;
}

// construct a active edge from a line
// -----------------------------------
ActEdge::ActEdge(Line line)
{   
    int x0 = line.vertex[0].x, y0 = line.vertex[0].y;
    int x1 = line.vertex[1].x, y1 = line.vertex[1].y;
    if (y0 == y1)
    {
        ymax = ymin = y0;
        interX = -1; deltaX = 0;
    }
    else
    {
        ymax = std::max(y0, y1);
        ymin = std::min(y0, y1);
        if (ymin == y0) interX = x0;
        else interX = x1;
        int a = y1 - y0;
        int minusb = x1 - x0;
        deltaX = (float)minusb/a;
    }
}

// construct a simple polygon from given lines
// -------------------------------------------
SimplePolygon::SimplePolygon(std::vector<Line> lines)
{
    if (!check_self_intersect(lines))
    {
        ymin = sh; ymax = -1;
        for (int i = 0; i < lines.size(); i ++)
        {
            ActEdge ae(lines[i]);
            if (ae.interX < 0)
            {
                int xL = std::min(lines[i].vertex[0].x, lines[i].vertex[1].x);
                int xR = std::max(lines[i].vertex[0].x, lines[i].vertex[1].x);
                ActEdge ael(xL, lines[i].vertex[0].y);
                ActEdge aer(xR, lines[i].vertex[0].y);
                edges.push_back(ael); edges.push_back(aer);
                ymin = std::min(ymin, lines[i].vertex[0].y);
                ymax = std::max(ymax, lines[i].vertex[0].y);
            }
            else
            {
                edges.push_back(ae);
                ymin = std::min(ymin, ae.ymin);
                ymax = std::max(ymax, ae.ymax);
            }    
        }
    }
    else
    {
        std::ostringstream s;
        s << "ERROR::SIMPLE_POLYGON::INPUT: Not Simple Polygon, self-intersection exists";
        throw EdgeIntersect(s.str());
    }
}

// scan-converting algorithm
// -------------------------
void SimplePolygon::scan_convert(Color c)
{
    std::vector<ActEdge> NET[ymax-ymin+1];

    for (int i = 0; i < edges.size(); i ++)
        NET[edges[i].ymin - ymin].push_back(edges[i]);

    std::list<ActEdge> AET;

    for (int i = 0; i <= ymax-ymin; i ++)
    {
        for (int j = 0; j < NET[i].size(); j ++)
        {
            int netX = int(NET[i][j].interX + 0.5);
            auto it = AET.begin();
            for (; it != AET.end(); it ++)
            {
                int itX = int(it->interX + 0.5);
                if (itX >= netX) break;
            }
            if (it != AET.end())
            {
                int x0 = int(it->interX+0.5), x1 = int(NET[i][j].interX+0.5);
                if (x0 == x1)
                {
                    if (it->ymax == i+ymin && it->ymax != it->ymin)
                        it = AET.erase(it);
                    else if (NET[i][j].deltaX > it->deltaX)
                        it ++;
                }
            }
            AET.insert(it, NET[i][j]);
        }

        auto itL = AET.begin();
        auto itR = AET.begin(); itR ++;
        for (; itL != AET.end() && itR != AET.end(); itR ++, itL ++)
        {
            Point interL((int)(itL->interX+0.5), i+ymin);
            Point interR((int)(itR->interX+0.5), i+ymin);
            draw_line(interL.x, interL.y, interR.x, interR.y, c);
            itL = itR; itR ++;
        }

        for (auto it = AET.begin(); it != AET.end();)
        {
            if (it->ymax == i+ymin) it = AET.erase(it);
            else {
                it->interX += it->deltaX;
                it ++;
            }
        }
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
            {
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
    if (key == GLFW_KEY_D && action == GLFW_PRESS)
    {
        if (rmod == 1)
        {

        }
        else
        {   // prepare
            if (vertices.size() > 2)
            {   // if polygon exists
                // construct last edge
                Line ledge(vertices[0], vertices[vertices.size()-1]);
                edges.push_back(ledge);
                rmod = 1;   // draw
            }
            else
            {
                std::cout << "ERROR::SIMPLE_POLYGON::INPUT: The number of vertices of polygon should at least 3" << std::endl;
            }
        }
    }  
}