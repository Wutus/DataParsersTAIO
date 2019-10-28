#pragma once

#include "TimeSeriesConfig.hpp"
#include <iostream>
#include <memory>

class TimeSeriesConfigReader
{
public:
	std::unique_ptr<TimeSeriesConfig> Read(std::istream &stream);

private:
	static const char COMMENT_SIGN;
};

