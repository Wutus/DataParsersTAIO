#include "TimeSeriesConfigReader.hpp"
#include <sstream>

const char TimeSeriesConfigReader::COMMENT_SIGN('#');

std::unique_ptr<TimeSeriesConfig> TimeSeriesConfigReader::Read(std::istream& stream)
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
	std::string matrixTypeString;
	sstream >> matrixTypeString;
	auto it = TimeSeriesConfig::STRING_MATRIX_TYPE_DICTIONARY.find(matrixTypeString);
	if (it == TimeSeriesConfig::STRING_MATRIX_TYPE_DICTIONARY.end())
	{
		throw std::invalid_argument("Unknown error type: " + matrixTypeString);
	}
	TimeSeriesConfig::MatrixType matrixType = it->second;
	unsigned int dataPortion;
	sstream >> dataPortion;
	unsigned int timeOffset;
	sstream >> timeOffset;
	return std::make_unique<TimeSeriesConfig>(matrixType, dataPortion, timeOffset);
}