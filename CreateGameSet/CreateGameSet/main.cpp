#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <codecvt>
#include <locale>
#include <vector>
#include <functional>

#include <Windows.h>
#undef max
#undef min

std::filesystem::path NormalizePath(const std::filesystem::path& fullPath, const std::filesystem::path& parentDir) {
    // 親ディレクトリがフルパスの先頭にあるかを確認
    if (fullPath.string().find(parentDir.string()) == 0) {
        auto result = std::filesystem::relative(fullPath, parentDir);
        result = result.parent_path() / result.stem();

        if (result.is_absolute()) {
            return result;
        }
        if (result.string().find("./") != 0 && result.string().find("../") != 0) {
            // パスが "./" または "../" で始まっていない場合、"./" を追加
            result = std::filesystem::path("./") / result;
        }
        return result;
    }
    else {
        throw std::runtime_error("Specified parent directory is not a parent of the full path");
    }
}

std::filesystem::path NormalizePath(const std::filesystem::path& fullPath) {
    std::filesystem::path result = fullPath;

    if (result.string().find("./") != 0 && result.string().find("../") != 0) {
        // パスが "./" または "../" で始まっていない場合、"./" を追加
        result = std::filesystem::path("./") / result;
    }
    return result;
}

void CopyFilesWithStructure(const std::filesystem::path& sourceDir, const std::filesystem::path& destDir, const std::function<bool(const std::filesystem::path&)>& filter) {
    for (const auto& entry : std::filesystem::recursive_directory_iterator(sourceDir)) {
        if (filter(entry.path())) {
            // 元のディレクトリ構造を保つための相対パスを作成
            std::filesystem::path relativePath = std::filesystem::relative(entry.path(), sourceDir);
            std::filesystem::path destPath = destDir / relativePath;

            // 必要なディレクトリを作成
            std::filesystem::create_directories(destPath.parent_path());

            // ファイルをコピー
            std::filesystem::copy(entry.path(), destPath, std::filesystem::copy_options::overwrite_existing);
        }
    }
}

int main() {
    try {
        std::ifstream file;
        file.open("./LoadResrouce.log");

        std::vector<std::filesystem::path> data;
        std::string line;
        while (std::getline(file, line)) {
            data.push_back(line);
        }

        CopyFilesWithStructure("./Projects/Game/Resources/", "../Game/Resources/", [&data](const std::filesystem::path& path){
            if (path.has_extension()) {
                auto&& tmp = NormalizePath(path, "./Projects/Game/").generic_wstring();
                for (auto& i : data) {
                    if (NormalizePath(i).generic_wstring() == tmp) {
                        return true;
                    }
                }
            }
            return false;
            }
        );
        CopyFilesWithStructure("./Projects/Game/Resources/Datas", "../Game/Resources/Datas", [](const std::filesystem::path& path) {
            return path.has_extension();
            }
        );

        CopyFilesWithStructure("./Projects/Game/Resources/Shaders", "../Game/Resources/Shaders", [](const std::filesystem::path& path) {
            return path.has_extension();
            }
        );

        CopyFilesWithStructure("../Generated/Outputs/Game/Release", "../Game/", [](const std::filesystem::path& path) {
            return path.has_extension() and path.extension().string() != ".pdb"; 
            }
        );

    }
    catch (const std::exception& err) {
        std::cout << err.what();
        return -1;
    }

    return 0;
}