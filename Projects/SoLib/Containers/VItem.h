#pragma once
#include "../Text/StaticString.h"
#include <ostream>
#include <variant>

namespace SoLib {
	namespace Containers {
		/// @brief 文字列付きラッパ型
		/// @tparam Str 文字列
		/// @tparam T 型名
		template<SoLib::Text::ConstExprString Str, typename T>
		struct VItem : public SoLib::Text::StaticString<Str> {

			VItem() = default;
			VItem(const T &right) : item(right) {}
			VItem(T &&right) : item(std::move(right)) {}

			inline static constexpr friend std::ostream &operator<< (std::ostream &stream, const VItem &item) { return stream << item.c_str() << ' ' << item.item; }

			T item;
			operator T &() { return item; }
			operator const T &() const { return item; }

			T *operator->() { return &item; }
			const T *operator->() const { return &item; }

			VItem &operator=(const T &right) { item = right; return *this; }
			VItem &operator=(T &&right) { item = std::move(right); return *this; }

			T &operator*() { return item; }
			const T &operator*() const { return item; }

		};



		template<typename... Ts>
		class VariantArray {
		public:
			constexpr VariantArray(Ts... args) : variants_{ std::variant<Ts...>(args)... } {}

			constexpr auto size() const noexcept {
				return sizeof...(Ts);
			}

			std::variant<Ts...> &operator[](size_t index) {
				return variants_[index];
			}

			const std::variant<Ts...> &operator[](size_t index) const {
				return variants_[index];
			}

			// Function to print VariantArray elements using std::to_string
			template<typename T>
			void printVariantArray(const T &t) const {
				for (const auto &val : variants_) {
					std::visit([&t](const auto &arg) {
						std::to_string(t.*arg);
						}, val);
				}
			}


		private:
			std::array<std::variant<Ts...>, sizeof...(Ts)> variants_;
		};
	}
	using namespace Containers;
}