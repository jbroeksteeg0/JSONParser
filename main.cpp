#include <iostream>
#include "JSONParser.h"

int main() {
	JSONParser::JSONObject res = JSONParser::fromFile("data.json");
	
	std::string temp = JSONParser::toString(res);
	JSONParser::JSONObject again = JSONParser::parseString(temp);
	temp = JSONParser::toString(again);
	
    return 0;
}
