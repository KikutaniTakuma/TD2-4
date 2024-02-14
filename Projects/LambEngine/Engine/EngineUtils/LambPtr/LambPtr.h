#pragma once
#include <type_traits>
#ifdef _DEBUG
#include "Utils/ConvertString/ConvertString.h"
#include <typeinfo>
#endif // _DEBUG


struct IUnknown;
struct ID3D12Object;

namespace Lamb {
	/// <summary>
	/// IUnknownを継承しているかつ、デストラクタにアクセス可能な型制限のコンセプト
	/// </summary>
	template<class T>
	concept IsIUnknownBased = std::is_base_of_v<IUnknown, T>&& requires(T a){
		{ a.~T() } -> std::convertible_to<void>;
	};

	/// <summary>
	/// <para>ComPtrに変わる自作スマートポインタ</para>
	/// </summary>
	template<IsIUnknownBased T>
	class LambPtr {
	/// <summary>
	/// コンストラクタ
	/// </summary>
	public:
		LambPtr() :
			ptr_{ nullptr }
		{}
		LambPtr(T* ptr) :
			LambPtr{}
		{
			*this = ptr;
		}
		LambPtr(std::nullptr_t ptr) :
			ptr_{ ptr }
		{}
		LambPtr(const LambPtr& right) :
			LambPtr{}
		{
			*this = right;
		}
		LambPtr(LambPtr&& right) noexcept :
			LambPtr{}
		{
			*this = std::move(right);
		}

	/// <summary>
	/// デストラクタ
	/// </summary>
	public:
		~LambPtr() {
			Delete();
		}

	/// <summary>
	/// 代入演算子
	/// </summary>
	public:
		LambPtr<T>& operator=(const LambPtr<T>& right) {
			this->Delete();

			this->ptr_ = right.ptr_;

			this->AddRef();

			return *this;
		}
		LambPtr<T>& operator=(LambPtr<T>&& right) noexcept {
			this->Delete();

			this->ptr_ = right.ptr_;

			this->AddRef();

			right.Delete();

			return *this;
		}
		LambPtr<T>& operator=(T* right) {
			this->Delete();

			this->ptr_ = right;

			return *this;
		}
		LambPtr<T>& operator=(std::nullptr_t right) {
			this->Delete();

			this->ptr_ = right;

			return *this;
		}

	/// <summary>
	/// 演算子のオーバーロード
	/// </summary>
	public:
		T* const operator->() {
			return ptr_;
		}
		T* const operator->() const {
			return ptr_;
		}

		T& operator*() {
			return *ptr_;
		}
		const T& operator*() const {
			return *ptr_;
		}

		explicit operator bool() const {
			return ptr_;
		}
		bool operator!() const {
			return !ptr_;
		}


		bool operator==(T* right) const {
			return ptr_ == right;
		}
		bool operator==(const LambPtr<T>& right) const {
			return ptr_ == right.ptr_;
		}

		bool operator!=(T* right) const {
			return ptr_ != right;
		}
		bool operator!=(const LambPtr<T>& right) const {
			return ptr_ != right.ptr_;
		}

	/// <summary>
	/// メンバ関数
	/// </summary>
	public:
		/// <summary>
		/// 管理するポインタがnullptrだったらtrueを返す
		/// </summary>
		/// <returns>nullptrだったらtrue</returns>
		bool Empty() const {
			return !(*this);
		}

		/// <summary>
		/// ポインタを取得する関数
		/// </summary>
		/// <returns>管理してるポインタ</returns>
		T* Get() {
			return ptr_;
		}

		/// <summary>
		/// ポインタを取得する関数
		/// </summary>
		/// <returns>管理してるポインタ</returns>
		T* const Get() const {
			return ptr_;
		}

		/// <summary>
		/// 管理してるポインタのポインタを取得する
		/// </summary>
		/// <returns>管理してるポインタのポインタ</returns>
		T** GetAddressOf() {
			return &ptr_;
		}
		/// <summary>
		/// 管理してるポインタのポインタを取得する
		/// </summary>
		/// <returns>管理してるポインタのポインタ</returns>
		T* const* GetAddressOf() const {
			return &ptr_;
		}

		/// <summary>
		/// リセット関数(実質Delete関数と同じなのでそのラッパー関数)
		/// </summary>
		void Reset() {
			Delete();
		}

		/// <summary>
		/// リセットして新しいポインタを設定する
		/// </summary>
		/// <param name="ptr">新しいポインタ</param>
		void Reset(T* ptr) {
			Delete();
			*this = ptr;
		}

		/// <summary>
		/// リセットして新しいポインタを設定する
		/// </summary>
		/// <param name="ptr">新しいポインタ</param>
		template<IsIUnknownBased U>
		void Reset(U* ptr) {
			Delete();
			*this = ptr;
		}

		/// <summary>
		/// <para>所持してるポインタを返して所有権を手放す</para>
		/// <para>この関数を呼んだら解放が呼ばれずnullptrになる</para>
		/// </summary>
		/// <returns>所持してるポインタ</returns>
		[[nodiscard]]
		T* Release() {
			T* tmp = ptr_;
			ptr_ = nullptr;

			return tmp;
		}

		template<class ClassName>
		void SetName() {
#ifdef _DEBUG
			if constexpr (std::is_base_of_v<ID3D12Object, T>) {
				if (ptr_) {
					std::wstring resourceName = ConvertString(
						std::string{ typeid(ClassName).name() } + " : " + std::string{ typeid(T).name() }
					);
					ptr_->SetName(resourceName.c_str());
				}
			}
#endif // _DEBUG
		}


	private:
		/// <summary>
		/// メンバを解放する
		/// </summary>
		void Delete() {
			if (ptr_) {
				ptr_->Release();
				ptr_ = nullptr;
			}
		}

		void AddRef() {
			if (ptr_) {
				ptr_->AddRef();
			}
		}

	/// <summary>
	/// メンバ変数
	/// </summary>
	private:
		/// <summary>
		/// 管理するポインタ
		/// </summary>
		alignas(8) T* ptr_;
	};
}