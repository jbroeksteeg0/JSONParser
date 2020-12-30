#include <iostream>
#include <fstream>
#include "JSONParser.h"

int main() {
	std::string data;
	std::ifstream in("data.json");
	std::getline(in,data);
	
	JSONParser::JSONObject res = JSONParser::fromFile("data.json");
	
	//res.print();
	
	std::cout << JSONParser::toString(res) << std::endl;
    return 0;
}
