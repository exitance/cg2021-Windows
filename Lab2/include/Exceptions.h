#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>
#include <string>
#include <iostream>

class EdgeIntersect
{
public:
    EdgeIntersect(std::string s="Edge Intersect")
        {msg = s;}
    void outputMsg() {std::cout << msg << std::endl;}
private:
    std::string msg;
};

#endif