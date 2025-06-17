#pragma once
#include <iostream>
#include <vector>
#include <string>
#include "DataStructure.h"


class Parser {
private:

	std::vector<ECommerceEvent> eventVector;
public:
	Parser();

	void parseFile(const std::string& file);

};

