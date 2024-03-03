#pragma once
#include "../Text/StaticString.h"
#include <ostream>

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
	}
	using namespace Containers;
}