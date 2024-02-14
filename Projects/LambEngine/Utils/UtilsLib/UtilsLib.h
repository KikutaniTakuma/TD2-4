#pragma once
#include <vector>
#include <filesystem>

/// <summary>
/// 便利なあれやこれや
/// </summary>
namespace Lamb {
	/// <summary>
	/// 特定のフォルダの特定の拡張子のファイルをすべて探して返す
	/// </summary>
	/// <param name="directoryName">ディレクトリ名(例 : "./Resources/")</param>
	/// <param name="extension">拡張子(例 : ".png")</param>
	/// <returns>ファイルのパスを格納したコンテナ</returns>
	std::vector<std::filesystem::path> GetFilePathFormDir(
		const std::filesystem::path& directoryName,
		const std::filesystem::path& extension
	);
}