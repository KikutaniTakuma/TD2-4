#include "CSV.h"

SoLib::IO::CSV::CSV(const File &file) {
	file >> *this;
}

std::vector<std::string> SoLib::IO::CSV::CreateLine(const std::string &str)
{
	std::vector<std::string> result;
	std::string cell = "";
	std::stringstream lineBuff = std::stringstream(str);
	while (std::getline(lineBuff, cell, ',')) {
		// 先頭の空白文字を削除
		size_t start = cell.find_first_not_of(" \t");
		if (start != std::string::npos) {
			cell = cell.substr(start);
		}

		// 終端の空白文字を削除
		size_t end = cell.find_last_not_of(" \t");
		if (end != std::string::npos) {
			cell = cell.substr(0, end + 1);
		}
		result.push_back(cell);
	}

	return result;
}

const SoLib::IO::File &SoLib::IO::operator>>(const SoLib::IO::File &file, SoLib::IO::CSV &csv) {
	std::stringstream fileData;
	fileData << file.GetData().rdbuf();

	// 最大の横幅
	size_t maxLength{};
	// 一時データ
	std::vector<std::vector<std::string>> tmp;
	// 行データ
	std::string line = "";
	while (std::getline(fileData, line)) {
		// 一行作る
		std::vector<std::string> &csvData = tmp.emplace_back(SoLib::IO::CSV::CreateLine(line));
		// 最大値を保存
		maxLength = (std::max)(maxLength, csvData.size());
	}

	csv.data_.Resize(tmp.size(), maxLength);
	for (size_t y = 0u; y < tmp.size(); y++) {
		for (size_t x = 0u; x < maxLength; x++) {
			csv[y][x] = std::move(tmp[y][x]);
		}
	}

	return file;
}