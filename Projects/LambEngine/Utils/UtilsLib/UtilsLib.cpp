#include "UtilsLib.h"

namespace Lamb {
	std::vector<std::filesystem::path> GetFilePathFormDir(
		const std::filesystem::path& directoryName,
		const std::filesystem::path& extension
	) {
		std::vector<std::filesystem::path> result;
		if (!std::filesystem::exists(directoryName)) {
			return result;
		}

		std::filesystem::directory_iterator directory{ directoryName };

		// ディレクトリ内を探索
		for (const auto& entry : directory) {
			const auto& path = entry.path();
			// もし拡張子がなかったら
			if (!path.has_extension()) {
				std::vector<std::filesystem::path> files;
				// その中にあるファイルをすべて探索(再帰的に探す)
				files = GetFilePathFormDir(path, extension);

				// コンテナに追加
				for (auto& i : files) {
					result.push_back(i);
				}
			}

			// 特定の拡張子を持つファイルだった場合コンテナに追加
			else if (path.extension() == extension) {
				result.push_back(path);
			}
		}

		return result;
	}
}