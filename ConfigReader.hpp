#pragma once

#include "SolvingConfig.hpp"
#include <iostream>
#include <string>
#include <unordered_map>
#include <stdexcept>
#include <memory>
#include <sstream>

class ConfigReader
{
public:
	ConfigReader() = default;
	~ConfigReader() = default;

	template<typename FloatType>
	std::unique_ptr<ns::SolvingConfig<FloatType>> Read(std::istream &stream);
private:
	static const std::unordered_map<std::string, ns::ErrorType> ERROR_DICTIONARY;
	static const char COMMENT_SIGN;
};

template<typename FloatType>
std::unique_ptr<ns::SolvingConfig<FloatType>> ConfigReader::Read(std::istream &stream)
{
	std::string input;
	do { 
		if (!std::getline(stream, input))
		{
			throw std::invalid_argument("Bad structure of input");
		}
		if (input.empty())
		{
			continue;
		}
		size_t first = input.find_first_not_of(" \t\r");
		if (first == std::string::npos)
		{
			input = "";
			continue;
		}
		size_t last = input.find_last_not_of(" \t\r");
		input = input.substr(first, (last - first + 1));
	} while (input.size() == 0 || input[0] == COMMENT_SIGN);
	std::istringstream sstream(input);
	std::string errorString;
	sstream >> errorString;
	auto it = ERROR_DICTIONARY.find(errorString);
	if (it == ERROR_DICTIONARY.end())
	{
		throw std::invalid_argument("Unknown error type: " + errorString);
	}
	ns::ErrorType errorType = it->second;
	FloatType maxError;
	sstream >> maxError;
	unsigned int maxIterations;
	sstream >> maxIterations;
	bool biased, activateInput, revertOnErrorIncrease;
	sstream >> activateInput;
	sstream >> biased;
	sstream >> revertOnErrorIncrease;
	return std::make_unique<ns::SolvingConfig<FloatType>>(errorType, maxError, maxIterations, activateInput, biased, revertOnErrorIncrease);
}