#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <chrono>
#include <glm/glm.hpp>

namespace console {

	inline void printInfo(std::string msg) {
		std::cout << "INFO: " << msg << std::endl;
	}

	inline void printWarn(std::string msg) {
		std::cout << "WARN: " << msg << std::endl;
	}

	void printError(std::string msg, bool fatal = false) {
		std::cout << "ERROR: " << msg << std::endl;
		if (fatal) {
			system("pause");
			exit(-1);
		}
	}

	void print(std::string msg, bool linebreak = true)
	{
		std::cout << msg << linebreak ? "\n" : "";
	}

	void linebreak(unsigned int count = 1)
	{
		for (unsigned int i = 0; i < count; i++) {
			std::cout << std::endl;
		}
	}

	std::string getInput()
	{
		std::string input;
		std::getline(std::cin, input);
		return input;
	}
}

namespace util {

	namespace chrono {
		typedef std::chrono::high_resolution_clock::time_point point;
		auto& now = std::chrono::high_resolution_clock::now;
		float deltaTime(point t1, point t2) {
			return std::chrono::duration_cast<std::chrono::duration<float>>(t2 - t1).count();
		}
	}

	std::vector<std::string> splitString(std::string strToSplit, char delimeter, bool preventEmpty = true)
	{
		std::stringstream ss(strToSplit);
		std::string item;
		std::vector<std::string> splittedStrings;
		while (std::getline(ss, item, delimeter))
		{
			if (!(item == "" && preventEmpty))
				splittedStrings.push_back(item);
		}
		return splittedStrings;
	}

	// Searches from the pos down to the first element and returns the position of a match. Returns -1 if there were no match
	template<typename T>
	int findVectorItem(std::vector<T> &vec, T value, int pos = 0) {

		auto it = std::find(vec.begin() + pos,vec.end(),value);
		if (it != vec.end()) {
			return std::distance(vec.begin(), it);
		}

		/*
		for (std::vector<T>::iterator i = vec.begin(); i != vec.end(); i++) {
			if (*i == value) {
				return i - vec.begin();
			}
		}*/
		return -1;
	}

	// Gets the extention of a path
	std::string fileExt(std::string AFile) {
		size_t i = AFile.find_last_of('.');
		if (i != std::string::npos)
		return AFile.substr(i);
		return "";
	}

	bool replaceChars(std::string& str, std::string from, char to) {
		size_t start_pos = str.find(from);
		if (start_pos == std::string::npos)
			return false;
		str.replace(start_pos, from.length(), "°");
		str.at(start_pos) = to;
		return true;
	}

	int countLines(std::ifstream &stream) {
		int result = 0;
		std::string str;
		stream.clear();
		stream.seekg(0);
		while (std::getline(stream, str)) {
			result++;
		}
		stream.clear();
		stream.seekg(0);
		return result;
	}
}

namespace language {

	const std::string ERROR_NO_STRING_FOUND = "##_NO_STRING_FOUND_##";

	class languageContainer {
	private:
		std::ifstream stringfile;
	public:
		languageContainer(std::string AFile) {
			stringfile.open(AFile);
			if (!stringfile.is_open()) {
				console::printError("LanguageContainer: Opening file failed | [" + AFile + "]");
			}
		}
		~languageContainer() {
			stringfile.close();
		}
		std::string getByTag(std::string ATag) {
			std::string tempstring;
			while (std::getline(stringfile, tempstring)) {
				if (int pos = (tempstring.find(ATag + "=")) != std::string::npos) {
					stringfile.seekg(0);
					std::string result = tempstring.substr(ATag.size() + 1, tempstring.size() - ATag.size() - 1);
					while (util::replaceChars(result, "\\n", '\n'));
					while (util::replaceChars(result, "\\t", '\t'));
					return result;
				}
			}
			stringfile.seekg(0);
			return ERROR_NO_STRING_FOUND;
		}
	};
}
