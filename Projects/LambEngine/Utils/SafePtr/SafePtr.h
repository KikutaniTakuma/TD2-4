#pragma once
#include <utility>
#include "Error/Error.h"
#include "../SafeDelete/SafeDelete.h"

namespace Lamb {
	/// <summary>
	/// <para>ヌルアクセスを許さないポインタ</para>
	/// <para>自動開放機能はない</para>
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template<typename T>
	class SafePtr {
	public:
		using type = T;
		static constexpr size_t size = sizeof(T);

	public:
		SafePtr():
			ptr_(nullptr)
		{};
		SafePtr(T* ptr) :
			ptr_(ptr)
		{}
		SafePtr(std::nullptr_t) :
			ptr_(nullptr)
		{}
		SafePtr(const SafePtr&) = default;
		SafePtr(SafePtr&&) = default;
		~SafePtr() = default;

	public:
		SafePtr& operator=(std::nullptr_t) {
			ptr_ = nullptr;

			return *this;
		}
		SafePtr& operator=(T* ptr) {
			ptr_ = ptr;

			return *this;
		}
		SafePtr& operator=(const SafePtr&) = default;
		SafePtr& operator=(SafePtr&&) = default;

	public:
		explicit operator bool() const {
			return !!ptr_;
		}
		bool operator!() const {
			return !ptr_;
		}

		bool operator==(const SafePtr& right) const {
			return ptr_ == right.ptr_;
		}
		bool operator!=(const SafePtr& right) const {
			return ptr_ != right.ptr_;
		}
		bool operator==(T* right) const {
			return ptr_ == right;
		}
		bool operator!=(T* right) const {
			return ptr_ != right;
		}

		T** operator&()  {
			return &ptr_;
		}

		T* operator->()  {
			NullPointerException(__func__);
			return ptr_;
		}
		const T* operator->() const  {
			NullPointerException(__func__);
			return ptr_;
		}

		T& operator*()  {
			NullPointerException(__func__);
			return *ptr_;
		}

		const T& operator*() const  {
			NullPointerException(__func__);
			return *ptr_;
		}

		T& operator[](size_t index)  {
			NullPointerException(__func__);
			return (ptr_[index]);
		}
		const T& operator[](size_t index) const  {
			NullPointerException(__func__);
			return (ptr_[index]);
		}

		// 後置インクリメント
		SafePtr operator++(int32_t) {
			SafePtr tmp = ptr_++;
			return tmp;
		}
		// 後置デクリメント
		SafePtr operator--(int32_t) {
			SafePtr tmp = ptr_--;
			return tmp;
		}

		// 前置インクリメント
		SafePtr& operator++() {
			++ptr_;
			return *this;
		}
		// 前置デクリメント
		SafePtr& operator--() {
			--ptr_;
			return *this;
		}

		/// <summary>
		/// 通常ポインタを取得
		/// </summary>
		/// <returns>ポインタ</returns>
		T* get() {
			return ptr_;
		}
		/// <summary>
		/// 通常ポインタを取得
		/// </summary>
		/// <returns>ポインタ</returns>
		const T* get() const {
			return ptr_;
		}

		/// <summary>
		/// 通常ポインタを取得し、nullptrを代入する
		/// </summary>
		/// <returns>ポインタ</returns>
		T* release() {
			T* result = ptr_;
			ptr_ = nullptr;
			return result;
		}

		/// <summary>
		/// 解放
		/// </summary>
		void reset() {
			Lamb::SafeDelete(ptr_);
		}
		/// <summary>
		/// 解放して新しいポインタを代入
		/// </summary>
		/// <param name="ptr">ポインタ</param>
		void reset(T* ptr) {
			reset();
			ptr_ = ptr;
		}

		void reset(const Lamb::SafePtr<T>& ptr) {
			reset();
			*this = ptr;
		}

	public:
		/// <summary>
		/// ヌルの場合は例外をthrowする
		/// </summary>
		/// <param name="funcName">関数名</param>
		template<class Name = SafePtr<T>>
		inline void NullPointerException(const std::string& funcName) const {
			if (not ptr_) {
				throw Lamb::Error::Code<Name>("NullPointerException", funcName);
			}
		}

	private:
		T* ptr_;
	};

	template<class T, class... Types>
	SafePtr<T> MakeSafePtr(Types&&... args) {
		return SafePtr<T>(new T(std::forward<Types>(args)...));
	}
}