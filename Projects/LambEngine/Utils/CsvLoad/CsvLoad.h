#pragma once
#include <vector>
#include <string>




/// <summary>
/// Csvローダー
/// </summary>
/// <param name="fileNmae">csvのファイル名</param>
/// <returns>ロードしたcsvの二次元配列</returns>
std::vector<std::vector<int32_t>> CsvLoad(const std::string& fileNmae);