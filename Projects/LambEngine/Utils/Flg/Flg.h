#pragma once
#include <concepts>

namespace Lamb {
	/// <summary>
	/// フラグ管理クラス
	/// </summary>
	class Flg {
	public:
		Flg();
		Flg(const Flg& right);
		Flg(Flg&& right) noexcept;
		Flg(bool right);
		~Flg();

		Flg& operator=(const Flg&) = default;
		Flg& operator=(Flg&&) = default;

		inline Flg& operator=(bool flg) {
			flg_ = flg;
			//preFlg_ = !flg_;
			return *this;
		}

		/// <summary>
		/// 暗黙型定義
		/// </summary>
		inline explicit operator bool() const {
			return flg_;
		}

		bool operator!() const {
			return !flg_;
		}

		/// <summary>
		/// 暗黙型定義(整数型)
		/// </summary>
		/// <typeparam name="T">整数型にのみ型制限</typeparam>
		template<std::integral T>
		inline explicit operator T() const {
			return static_cast<T>(flg_);
		}


		inline bool operator==(const Flg& other) const {
			return flg_ == other.flg_ && preFlg_ == other.preFlg_;
		}

		inline bool operator!=(const Flg& other) const {
			return flg_ != other.flg_ || preFlg_ != other.preFlg_;
		}

		inline bool* const data() {
			return &flg_;
		}
		inline const bool* const data() const {
			return &flg_;
		}

	public:
		/// <summary>
		/// アップデート(Managerが自動でやるので呼ばなくて良い)
		/// </summary>
		void Update();

		/// <summary>
		/// trueになった瞬間を返す
		/// </summary>
		/// <returns>trueになった瞬間にtrue</returns>
		bool OnEnter() const {
			return flg_ && !preFlg_;
		}

		/// <summary>
		/// trueになっている間
		/// </summary>
		/// <returns>trueになっている間はtrue</returns>
		bool OnStay() const {
			return flg_ && preFlg_;
		}

		/// <summary>
		/// falseになった瞬間
		/// </summary>
		/// <returns>falseになった瞬間にtrue</returns>
		bool OnExit() const {
			return !flg_ && preFlg_;
		}

	private:
		bool flg_;
		bool preFlg_;
	};
}