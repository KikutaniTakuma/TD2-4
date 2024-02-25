#pragma once
#include <vector>
#include <cstdint>
#include "../SoLib/SoLib/SoLib_Traits.h"

namespace SoLib {
	namespace Containers {

		template<SoLib::IsRealType T>
		class Vector2D {
		public:
			using LineType = std::vector<T>;
			using MapType = std::vector<LineType>;
			using iterator = MapType::iterator;
			using const_iterator = MapType::const_iterator;

			Vector2D() = default;
			~Vector2D() = default;

			LineType &operator[](const size_t index) {
				return data_[index];
			}
			const LineType &operator[](const size_t index) const {
				return data_[index];
			}

			LineType &at(const size_t index) {
				return data_.at(index);
			}
			const LineType &at(const size_t index) const {
				return data_.at(index);
			}

			MapType &Get() { return data_; }
			const MapType &Get() const { return data_; }

			auto begin() { return data_.begin(); }
			auto begin() const { return data_.cbegin(); }
			auto cbegin() const { return data_.cbegin(); }

			auto end() { return data_.end(); }
			auto end() const { return data_.cend(); }
			auto cend() const { return data_.cend(); }

			size_t GetWidth() const { return width_; }
			size_t GetHeight() const { return height_; }

			void clear() {
				width_ = 0u;
				height_ = 0u;
				data_.clear();
			}

			void push_back(const LineType &line) {
				data_.push_back(line);
			}

			void push_back(LineType &&line) {
				data_.push_back(std::move(line));
			}

		private:
			// 横幅
			size_t width_;
			// 縦幅
			size_t height_;

			MapType data_;
		};

	}
}