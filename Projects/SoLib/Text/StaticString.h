#pragma once
#include <cstdint>
#include <array>
#include <ostream>
#include <string_view>
#include <string>

namespace SoLib {
	namespace Text {

		/// @brief コンパイル時定義固定文字列
		/// @tparam Size 文字列長
		template <uint32_t Size>
		struct ConstExprString {
			static constexpr uint32_t length_ = Size - 1;
			std::array<char, Size> buf_;

			// 文字列リテラルから推論を行うためのコンストラクタ
			constexpr ConstExprString(const char(&s_literal)[Size]) {
				for (uint32_t i = 0u; i < Size; i++) {
					buf_[i] = s_literal[i];
				}
				buf_[length_] = '\0'; // 終端文字を追加
			}

			constexpr std::string_view view() const noexcept { return { buf_.data(), length_ + 1 }; }
			constexpr const char *const c_str() const noexcept { return buf_.data(); }
		};

		/// @brief 静的固定文字列
		/// @tparam Str 文字列
		template <ConstExprString Str>
		struct StaticString {
			static constexpr ConstExprString str_ = Str;
		public:

			constexpr std::string_view view() const noexcept { return str_.view(); }

			constexpr const char *const c_str() const noexcept { return str_.c_str(); }
			inline static constexpr friend std::ostream &operator<< (std::ostream &ost, const StaticString str) { return ost << str.c_str(); }
		};
	}
}