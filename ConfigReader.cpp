#include "ConfigReader.hpp"

const std::unordered_map<std::string, ns::ErrorType> ConfigReader::ERROR_DICTIONARY{
	{"MAX_DIFFERENCE", ns::ErrorType::MAX_DIFFERENCE},
	{"MEAN", ns::ErrorType::MEAN},
	{"MEAN_SQUARED", ns::ErrorType::MEAN_SQUARED}
};

const char ConfigReader::COMMENT_SIGN('#');