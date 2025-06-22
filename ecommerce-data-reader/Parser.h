#pragma once
#include "DataStructure.h"
#include <vector>
#include <string>

class Parser {
public:
    Parser();
    void parseFile(const std::string& fileName);
    void runUnitTests();
    const std::vector<ECommerceEvent>& getEventVector() const;

private:
    std::vector<ECommerceEvent> eventVector;
};
