#include "Template.hpp"

// Constructors and Destructor
Template::Template() {
    std::cout << "Template default Constructor called" << std::endl;
    this->name = "Template";
}

Template::Template(std::string n) {
    std::cout << "Template Constructor called" << std::endl;
    this->name = n;
}

Template::~Template() {
    std::cout << "Template Destructor called" << std::endl;
}

// Canonical form
Template &Template::operator=(const Template &obj) {
    //copy depending on vars;
    return *this;
}

Template::Template(const Template &obj) {
    *this = obj;
}
