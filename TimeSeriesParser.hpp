#pragma once

#include "StringFloatConverts.hpp"
#include "Problem.hpp"
#include "TimeSeriesConfig.hpp"
#include "boost/tokenizer.hpp"
#include <iostream>
#include <unordered_map>

class TimeSeriesParser
{
public:
	TimeSeriesParser() = default;
	~TimeSeriesParser() = default;


	template<typename FloatType>
	//dataPortion - number of rows in one portion
	//timeOffset - offset between output and expected output further in time
	std::unique_ptr<ns::Problem<FloatType>> Read(std::istream& stream, const TimeSeriesConfig &config);
};

template<typename FloatType>
std::unique_ptr<ns::Problem<FloatType>> TimeSeriesParser::Read(std::istream& stream, const TimeSeriesConfig &config)
{
	unsigned int rowCount; //rows
	unsigned int columnCount; //columns

	unsigned int dataPortion = config.dataPortion;
	unsigned int timeOffset = config.timeOffset;
	TimeSeriesConfig::MatrixType matrixType = config.matrixType;

	std::vector<std::vector<FloatType>> inputMatrix;

	std::string line;
	boost::char_separator<char> sep{ " ," };
	//First run
	{
		inputMatrix.push_back(std::vector<FloatType>());
		std::getline(stream, line);
		boost::tokenizer<boost::char_separator<char>> tok{ line, sep };
		for (const auto& t : tok)
		{
			inputMatrix[0].push_back(fromString<FloatType>(t));
		}
	}

	columnCount = inputMatrix[0].size();

	while (true)
	{
		if (!std::getline(stream, line))
		{
			break;
		}

		inputMatrix.push_back(std::vector<FloatType>());
		inputMatrix[inputMatrix.size() - 1].reserve(columnCount);

		boost::tokenizer<boost::char_separator<char>> tok{ line, sep };
		for (const auto& t : tok)
		{
			inputMatrix[inputMatrix.size() - 1].push_back(FloatType(std::stod(t)));
		}

		if (inputMatrix[inputMatrix.size() - 1].size() != columnCount)
		{
			throw std::invalid_argument("Lines in entry file have variable number of arguments. First has " + std::to_string(columnCount) +
				", but " + std::to_string(inputMatrix.size()) + ". has " + std::to_string(inputMatrix[inputMatrix.size() - 1].size()));
		}
	}

	rowCount = inputMatrix.size();

	unsigned int matrix_size = dataPortion * columnCount;
	std::unique_ptr<ns::Problem<FloatType>> res = std::make_unique<ns::Problem<FloatType>>(matrix_size, rowCount - dataPortion - timeOffset + 1);

	for (unsigned int i = 0; i < rowCount; ++i)
	{
		for (unsigned int j = 0; j < columnCount; ++j)
		{
			stream >> inputMatrix[i][j];
		}
	}

	for (unsigned int i = 0; i < rowCount - dataPortion - timeOffset + 1; ++i)
	{
		for (unsigned int j = 0; j < dataPortion; ++j)
		{
			for (unsigned int k = 0; k < columnCount; ++k)
			{
				res->input[i][j * columnCount + k] = inputMatrix[i + j][k];
				res->output[i][j * columnCount + k] = inputMatrix[i + j + timeOffset][k];
			}
		}
	}

	switch (matrixType)
	{
	case TimeSeriesConfig::MatrixType::FULL:
		for (unsigned int i = 0; i < matrix_size; ++i)
		{
			for (unsigned int j = 0; j < matrix_size; ++j)
			{
				res->nonZeros.set(i, j, true);
			}
		}
		break;
	case TimeSeriesConfig::MatrixType::CHRONOLOGIC:
		for (unsigned int i = 0; i < matrix_size; ++i)
		{
			for (unsigned int j = 0; j < matrix_size; ++j)
			{
				res->nonZeros.set(i, j, (i % dataPortion) >= (j % dataPortion));
			}
		}
		break;
	case TimeSeriesConfig::MatrixType::INDEPENDENT:
		for (unsigned int i = 0; i < matrix_size; ++i)
		{
			for (unsigned int j = 0; j < matrix_size; ++j)
			{
				res->nonZeros.set(i, j, false);
			}
		};
		for (unsigned int i = 0; i < columnCount; ++i)
		{
			for (unsigned int j = 0; j < dataPortion; ++j)
			{
				for (unsigned int k = 0; k <= j; ++k)
				{
					res->nonZeros.set(i * dataPortion + j, i * dataPortion + k, true);
				}
			}
		}
		break;
	case TimeSeriesConfig::MatrixType::CUSTOM:
		break;
	}
	return res;
}