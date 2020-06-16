#include <iostream>
void log (std::ostream& output, const std::string& text = "notext") {
    output << text << std::endl;
}