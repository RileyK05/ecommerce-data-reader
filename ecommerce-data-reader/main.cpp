#include <iostream>
#include "Parser.h"

int main() {
    Parser parser;
    std::string fileToRead = "2019-Nov.csv";
    parser.parseFile(fileToRead);
}
