#pragma once
#include <string>
#include <vector>
#include "File.h"
#include "../Containers/Array2D.h"

namespace SoLib {
	namespace IO {


		class CSV final {
		public:

			using MapType = SoLib::Containers::Array2D<std::string>;
			using LineType = std::string *;
			using iterator = MapType::iterator;
			using const_iterator = MapType::const_iterator;

			CSV() = default;
			CSV(const File &file);
			~CSV() = default;

			LineType operator[](const size_t index) {
				return data_[index];
			}

			MapType &Get() { return data_; }
			const MapType &Get() const { return data_; }

			auto begin() { return data_.begin(); }
			auto begin() const { return data_.cbegin(); }
			auto cbegin() const { return data_.cbegin(); }

			auto end() { return data_.end(); }
			auto end() const { return data_.cend(); }
			auto cend() const { return data_.cend(); }

			size_t GetWidth() const { return data_.GetCols(); }
			size_t GetHeight() const { return data_.GetRows(); }

			friend const File &operator>>(const File &file, CSV &csv);

			MapType::View view() { return data_.view(); }
			MapType::ConstView view() const { return data_.view(); }

		private:

			static std::vector<std::string> CreateLine(const std::string &str);

		private:

			MapType data_;

		};


		inline std::ostream &operator<<(std::ostream &stream, const CSV &value) {
			for (const auto &line : value) {
				for (const auto &item : line) {
					stream << item << ", ";
				}
				stream.seekp(-2, std::ios_base::cur);
				stream << "\n";
			}
			return stream;
		}
	}
}