#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <cassert>
#include <fstream>

enum ValueType {
	Dictionary,
    Array,
    Number,
	String,
    Null
};

namespace JSONParser {
    extern std::string inputString;
    extern std::string::iterator inputIter;
class JSONObject {
private:
    std::string valueString;
    std::map<JSONObject, JSONObject> children;
public:
    ValueType type;
    JSONObject();
    JSONObject(ValueType);
	JSONObject(std::string);
	JSONObject(float);
    bool operator<(const JSONObject& rhs) const;
	JSONObject& operator[](float);
    JSONObject& operator[](std::string);
    JSONObject& operator[](JSONObject);
	void append(ValueType);
	void append(JSONObject);
    void append(std::string);
    void append(float);
    void append(bool);
    int size();
    std::vector<JSONObject> keys();
	float getValueNumber();
    std::string getValueString();
    void toString(std::string&);
	void print(int,bool,bool);
};

void advanceWhitespace();
std::string internal_parseString();
JSONObject internal_parseNull();
JSONObject internal_parseBool();
float internal_parseNumber();
JSONObject internal_parseArray();
JSONObject internal_parseDictionary();
JSONObject parseString(std::string);
JSONObject fromFile(std::string);
std::string toString(JSONObject);
}
