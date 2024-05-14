#include "CsvLoad.h"
#include "Error/Error.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <algorithm>

std::vector<std::vector<int32_t>> CsvLoad(const std::string& fileName) {
	std::vector<std::vector<int32_t>> result;

	if (!(std::filesystem::path(fileName).extension() == ".csv")) {
		throw Lamb::Error::Code<Lamb::Error::Function>("This file is not csv -> " + fileName, ErrorPlace);
	}
	else if(std::filesystem::exists(fileName)){
		throw Lamb::Error::Code<Lamb::Error::Function>("This file is not exists -> " + fileName, ErrorPlace);
	}

	std::ifstream file{ fileName };

	std::string line;
	while (std::getline(file, line))
	{
		std::stringstream sLine{ line };
		std::string stringBuf;

		result.push_back(std::vector<int32_t>());

		while (std::getline(sLine, stringBuf, ',')) {
			if (std::any_of(stringBuf.begin(), stringBuf.end(), isdigit)) {
				result.back().push_back(std::atoi(stringBuf.c_str()));
			}
		}
	}

	return result;
}