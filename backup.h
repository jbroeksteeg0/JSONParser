#pragma once

#include <iostream>
#include <fstream>
#include <map>
#include <cassert>
#include <vector>

#define mAssert(x,m) {\
if (!(x)) {\
	std::cout << "Assertion failed on line " << __LINE__ << " in " << __FILE__ << ": '" << m << "'" << std::endl;\
	exit(1);\
}\
}

enum ValueType {
	Dictionary,
	Array,
	Number,
	String,
	Null
};

namespace JSONParser {

class JSONObject {
private:
	std::string valueString;
	std::map<JSONObject, JSONObject> children;
public:
	ValueType type;
		
	JSONObject() {type = Null;}
	JSONObject(ValueType t): type(t) {}
	JSONObject(std::string s) {
		type = String;
		valueString=s;
	}
	JSONObject(float f) {
		type = Number;
		valueString = std::to_string(f);
	}

	bool operator<(const JSONObject& rhs) const {
		return valueString<rhs.valueString;
	}
	JSONObject& operator[](float ind) { // array or dict
		mAssert(type == Array || type == Dictionary, "Cannot index number or string");
		if (type == Array) {
			auto iter = children.find(JSONObject(ind));
			mAssert(iter != children.end(), "Could not find array index");
			return iter->second;
		} else if (type == Dictionary) {
			auto iter = children.find(JSONObject(ind));
			if (iter == children.end()) {
				children.insert({ind, JSONObject()});
				iter = children.find(JSONObject(ind));
			}
			return iter->second;
		}
		return children.begin()->second;
	}
	JSONObject& operator[](std::string key) { // dict
		auto iter = children.find(JSONObject(key));
		if (iter == children.end()) {
			children.insert({key, JSONObject()});
			iter = children.find(JSONObject(key));
		}
		return iter->second;
	}
	JSONObject& operator[](JSONObject key) { // dict
		mAssert(key.type == String || key.type == Number, "Must have number or string as key");
		auto iter = children.find(JSONObject(key));
		if (iter == children.end()) {
			children.insert({key, JSONObject()});
			iter = children.find(JSONObject(key));
		}
		return iter->second;	
	}
	
	void append(ValueType t) {
		mAssert(t == Array || t == Dictionary, "Cannot push back no value");
		mAssert(type == Array, "Append must be called on array");
		children[ JSONObject(children.size()) ] = JSONObject(t);
	}
	void append(JSONObject value) {
		mAssert(type == Array, "Append must be called on array");
		children[ JSONObject(children.size()) ] = value;
	}
	void append(std::string value) {
		mAssert(type == Array, "Append must be called on array");
		children[ JSONObject(children.size()) ] = JSONObject(value);
	}
	void append(float value) {
		mAssert(type == Array, "Append must be called on array");
		children[ JSONObject(children.size()) ] = JSONObject(value);
	}
	void append(bool value) {
		mAssert(type == Array, "Append must be called on array");
		children[ JSONObject(children.size()) ] = JSONObject(value);
	}
	
	int size() {
		mAssert(type == Array || type == Dictionary, "Size can only be called on a dictionary or string");
		return children.size();
	}
		
	std::vector<JSONObject> keys() {
		mAssert(type == Dictionary, "Must call keys() on dictionary");
		std::vector<JSONObject> res;
		for (auto iter: children) {
			res.push_back(iter.first);
		}
		return res;
	}
	float getValueNumber() {
		return std::stof(valueString);
	}
	std::string getValueString() {
		return valueString;
	}
	
	void toString(std::string& curr) {
		if (type==Array) {
			curr+="[";
			for (int i = 0; i < size(); i++) {
				children[i].toString(curr);
				if (i!=size()-1) {
					curr+=",";
				}
			}
			curr += "]";
		} else if (type == Dictionary) {
			curr += "{";
			for (int i = 0; i < size(); i++) {
				keys()[i].toString(curr);
				curr+=":";
				children[keys()[i]].toString(curr);
				if (i!=size()-1) {
					curr += ",";
				}
			}
			curr+="}";
		} else if (type == String) {
			curr += '"';
			for (char ch: valueString) {
				if (ch=='"' || ch =='\\') curr += '\\';
				curr+=ch;
			}
			curr += '"';
		} else if (type == Number) {
			curr += valueString;
		} else if (type == Null) {
			curr += "null";
		}
	}
	
	void print(int indent=0, bool doComma=false, bool skipFirst=false) {
		if (type==Array) {
			for (int i = 0; i < (skipFirst?0:indent); i++) {
				std::cout << " ";
			}
			if (size()==0) {
				std::cout << "[]" << (doComma?",":"") << std::endl;
				return;
			}
			std::cout << "[" << std::endl;
			
			for (int i = 0; i < size(); i++) {
				children[i].print(indent+2,i!=size()-1);
			}
			for (int i = 0; i < indent; i++) {
				std::cout << " ";
			}
			std:: cout << "]" << (doComma?",":"") << std::endl;
		} else if (type == Dictionary) {
			for (int i = 0; i < (skipFirst?0:indent); i++) {
				std::cout << " ";
			}
			if (size()==0) {
				std::cout << "{}" << (doComma?",":"") << std::endl;
				return;
			}
			std::cout << "{" << std::endl;
			
			for (int i = 0; i < size(); i++) {
				for (int j = 0; j < indent+2; j++) std::cout << " ";
				if (keys()[i].type == Number) std::cout << keys()[i].getValueNumber() << ": ";
				if (keys()[i].type == String) std::cout << "\"" << keys()[i].getValueString() << "\": ";
				children[ keys()[i] ].print(indent+2, i!=size()-1, true);
			}
			for (int i = 0; i < indent; i++) {
				std::cout << " ";
			}
			std:: cout << "}" << (doComma?",":"") << std::endl;
		} else if (type == Number) {
			for (int i = 0; i < (skipFirst?0:indent); i++) std::cout << " ";
			std::cout << getValueNumber() << (doComma?",":"") << std::endl;
		} else if (type == String) {
			for (int i = 0; i < (skipFirst?0:indent); i++) std::cout << " ";
			std::cout << "\"" << getValueString() << "\"" << (doComma?",":"") << std::endl;
		} else if (type == Null) {
			for (int i = 0; i < (skipFirst?0:indent); i++) std::cout << " ";
			std::cout << "NULL" << (doComma?",":"") << std::endl;
		}
	}
};

std::string inputString;
std::string::iterator inputIter;

void advanceWhitespace() {
	while (inputIter < inputString.end() && *inputIter == ' ') {
		inputIter++;
	}
}
std::string internal_parseString() {
	mAssert(inputIter < inputString.end(), "Parsing string: iter out of bounds");
	mAssert(*inputIter == '"', "Parsing string: function started on " + std::to_string(*inputIter));
	
	std::string ret="";
	
	inputIter++;
	while (*inputIter != '"') {
		if (*inputIter == '\\' && (inputIter+1)<inputString.end()) {
			ret += *(inputIter+1);
			inputIter+=2;
		} else {
			ret += *inputIter;
			inputIter++;
		}
	}
	inputIter++;
	return ret;
}

JSONObject internal_parseNull() {
	inputIter+=4;
	return JSONObject(Null);
}
JSONObject internal_parseBool() {
	if (*inputIter == 't') {
		inputIter+=4;
		return JSONObject(1);
	} else {
		inputIter+=5;
		return JSONObject(0);
	}
}

float internal_parseNumber() {
	mAssert(inputIter < inputString.end(), "Parsing number: iter out of bounds");
	mAssert(isdigit(*inputIter), "Parser number: must start on digit, got " + std::string(1,*inputIter));
	int sinceDecimal=-1; // not seen a decimal
	float res=*inputIter - '0';
	inputIter++;
	
	while (isdigit(*inputIter) || *inputIter == '.') {
		if (isdigit(*inputIter)) {
			if (sinceDecimal!=-1) {
				sinceDecimal++;
			}
			res*=10;
			res += *inputIter-'0';
		} else {
			mAssert(sinceDecimal==-1, "Found two decimal points in number");
			sinceDecimal=0;
		}
		inputIter++;
	}
	for (int i = 0; i < sinceDecimal; i++) res/=10;
	return res;
}

JSONObject internal_parseArray();

JSONObject internal_parseDictionary() {
	mAssert(inputIter < inputString.end(), "Parsing dictionary: iter out of bounds");
	mAssert(*inputIter == '{', "Parsing dictionary: function started on " + std::to_string(*inputIter));
	
	JSONObject res(Dictionary);
	inputIter++;
	advanceWhitespace();
	if (*inputIter == '}') {
		inputIter++;
		return res;
	}
	while (true) {
		// key : value,
		advanceWhitespace();
		char curr = *inputIter;
		JSONObject key, value;
		if (curr == '[') {
			key = internal_parseArray();
		} else if (curr == '{') {
			key = internal_parseDictionary();
		} else if (curr == '"') {
			key = internal_parseString();
		} else {
			key = internal_parseNumber();
		}
		advanceWhitespace();
		mAssert(*inputIter == ':', "Could not find ':'");
		inputIter++;
		advanceWhitespace();
		curr = *inputIter;

		// VALUE
		if (curr == '[') {
			value = internal_parseArray();
		} else if (curr == '{') {
			value = internal_parseDictionary();
		} else if (curr == '"') {
			value = internal_parseString();
		} else if (curr == 'n') {
			value = internal_parseNull();
		} else if (isdigit(curr)){
			value = internal_parseNumber();
		} else if (curr == 't' || curr == 'f') {
			value = internal_parseBool();
		}
		res[key] = value;
		mAssert(*inputIter == '}' || *inputIter == ',', "Incorrect character after passing dict item, got '" + std::string(1,*inputIter) + "'");
		if (*inputIter == '}') {
			inputIter++;
			break;
		} else {
			inputIter++;
		}
	}
	return res;
}
// inputIter is pointing to the opening bracket
JSONObject internal_parseArray() {
	mAssert(inputIter < inputString.end(), "Parsing array: iter out of bounds");
	mAssert(*inputIter == '[', "Parsing array: function started on " + std::to_string(*inputIter));
	
	JSONObject res(Array);
	inputIter++;
		
	advanceWhitespace();
	
	if (*inputIter == ']') { // empty array
		inputIter++;
		return res;
	}
	while (true) {
		advanceWhitespace();
		char curr = *inputIter;
		mAssert(curr == '[' || curr == '{' || curr == '"' || isdigit(curr), "Unexpected character: " + std::to_string(curr));
			
		if (curr == '"') {
			res.append(internal_parseString());
		} else if (curr == '[') {
			res.append(internal_parseArray());
		} else if (curr == '{') {
			res.append(internal_parseDictionary());
		} else if (curr == 'n') {
			res.append(internal_parseNull());
		} else if (curr == 't' || curr == 'f') {
			res.append(internal_parseBool());
		} else if (isdigit(curr)) {
			res.append(internal_parseNumber());
		}
			
		advanceWhitespace();
		if (*inputIter == ']') {
			inputIter++;
			break;
		} else {
			mAssert(*inputIter == ',', "Unexpected character on index " + std::to_string(inputIter-inputString.begin()-1) + ": " + std::string(1,*inputIter));
			inputIter++;
		}
	}
	
	return res;
}

JSONObject parseString(std::string input) {
	mAssert(input.length(), "Input string empty");
	
	inputString= "["+input+"]";
	inputIter = inputString.begin();
	return internal_parseArray()[0];
}

JSONObject fromFile(std::string path) {
	std::string res="";
	std::string raw="", temp="";
	std::ifstream in(path);
	while (std::getline(in, temp)) {
		if (raw!="") raw+="\n";
		raw+=temp;
	}
	bool inString=false;
	for (int i = 0; i < raw.length(); i++) {
		if (i > 0 && raw[i] == '"' && raw[i-1] == '\\') { // escaped quote
			res += raw[i];
		} else if (raw[i] == '"') { // nonescaped quote
			res+=raw[i];
			inString=!inString;
		} else if (raw[i] != '\n' || inString) { // other character
			res += raw[i];
		}
	}
	return parseString(res);
}

std::string toString(JSONObject root) {
	std::string res="";
	root.toString(res);
	return res;
}

};
