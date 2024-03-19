#ifndef Template_HPP
#define Template_HPP

#include <iostream>
#include <string>

class Template {
public:
    Template();
    Template(std::string);
    ~Template();

    // Copy constructor
    Template(const Template &obj);
    // Operator overload
    Template &operator=(const Template &obj);

private:
    std::string name;
};

#endif
