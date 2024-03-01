#pragma once
#include <unordered_map>
#include <unordered_set>
#include <typeindex>
#include <memory>
#include <type_traits>
#include <string>

#include "Camera/Camera.h"
#include ""
#include "../../Engine/DirectBase/Render/Camera.h"
#include "../../Utils/SoLib/SoLib.h"
//#include "../../Engine/DirectBase/Render/Camera.h"

class GameObject;
//class IScene;

///	
/// ↓ コンポーネントの最小構成 ↓
/// 
///	class MyComponent : public IComponent {	← IComponentを継承する
///	public:
///		using IComponent::IComponent;	←	IComponentのコンストラクタを引き継ぐ
///		
///		↓ ここからは自由 ↓
/// 
///		~MyComponent() override {}
/// 
///		[ Init() や Update() などはここに書く ]
///	
///	private:
///	
///	};
///

/// @brief コンポーネント指向の基盤
class IComponent {
	// 通常のコンストラクタは無効化
	IComponent() = delete;
	IComponent(const IComponent &) = delete;
	IComponent operator=(const IComponent &) = delete;
public:
	/// @brief 自動的に呼ばれるコンストラクタ
	/// @param object 紐づけられる実体のアドレス
	IComponent(GameObject *const object);
	virtual ~IComponent() = default;


	/// @brief Objectに追加した時に走る処理
	inline virtual void Init() {};
	/// @brief 任意で実行する初期化
	inline virtual void Reset() {};
	/// @brief 毎フレーム実行される処理
	inline virtual void Update() {};
	/// @brief 描画処理
	/// @param vp Cameraクラス
	inline virtual void Draw(const Camera<Render::CameraType::Projecction> &) const {};

	/// @brief jsonからの読み込み
	/// @param groupName グループ名
	inline virtual void ApplyVariables(const char *const groupName) { groupName; }

	/// @brief jsonへの紐づけ
	/// @param groupName グループ名
	inline virtual void AddVariable(const char *const groupName) const { groupName; }


	/// @brief ImGuiで表示する内容
	inline virtual void ImGuiWidget() {};

	/// @brief 接触時に実行される関数
	/// @param Object* other : 接触相手のアドレスが代入される
	inline virtual void OnCollision(GameObject *const) {};

	/// @brief デルタタイムを取得する
	/// @return 前フレームからの時間差分
	float GetDeltaTime() const;

	// 紐づけられた実体
	GameObject *const object_ = nullptr;
	BaseTransform *const transform_;

	float monoTimeScale_ = 1.f;
};

class GameObject {
	// 生きているか
	bool isActive_ = false;
	// コンポーネントの連想コンテナ
	std::unordered_map<std::type_index, std::unique_ptr<IComponent>> componentMap_;

	std::unordered_set<std::string> tag_;

	// 今後実装予定
	// 
	// シーン情報
	// IScene * scene_ = nullptr;
	// 
	// 親子関係
	// Object *parent_ = nullptr;
	// std::list<std::unique_ptr<Object>> children_;

	float deltaTime_{};

	float timeScale_ = 1.f;
public:
	// オブジェクトのSRT
	BaseTransform transform_;

	GameObject() = default;
	//Object(const Object&) = default;
	virtual ~GameObject() = default;

	virtual void Init();
	virtual void Reset();
	virtual void Update(float deltaTime);
	virtual void Draw(const Camera<Render::CameraType::Projecction> &vp) const;

	/*template<typename T>
	bool HasComponent();*/

	/// @brief コンポーネントを追加
	/// @tparam T コンポーネントの型
	/// @return コンポーネントのポインタ
	template <SoLib::IsBased<IComponent> T>
	inline T *const AddComponent();

	/// @brief コンポーネントの取得
	/// @tparam T コンポーネント型
	/// @return コンポーネントのポインタ (存在しない場合、nullptr)
	template <typename T>
	T *const GetComponent() const;

	/// @brief 生存しているかを設定する
	/// @param newState 生きている場合 [ true ]
	inline void SetActive(const bool newState) { isActive_ = newState; }

	/// @brief 生存しているかを取得する
	/// @return 生きている場合 [ true ]
	inline bool GetActive() const { return isActive_; }

	/// @brief タグの追加
	/// @param tag 追加するタグの文字列
	inline void AddTag(const std::string &tag) { tag_.insert(tag); }

	/// @brief タグを持っているか
	/// @param tag 検索するタグ
	/// @return タグを持っている場合 [ true ]
	inline bool HasTag(const std::string &tag) const { return tag_.contains(tag); }

	/// @brief タグの削除
	/// @param tag 削除するタグ
	inline 	void RemoveTag(const std::string &tag) { tag_.erase(tag); }

	const Vector3 &GetWorldPos();

	virtual void OnCollision(GameObject *const other);

	void ImGuiWidget();

	inline void SetTimeScale(const float timeScale) { timeScale_ = timeScale; }

	inline float GetDeltaTime() const { return deltaTime_ * timeScale_; }

private:

};

template <SoLib::IsBased<IComponent> T>
T *const GameObject::AddComponent() {
	static_assert(std::is_base_of<IComponent, T>::value, "テンプレート型はIComponentクラスの派生クラスではありません");

	// 既に存在する場合はその場で終了
	T *const findComp = GetComponent<T>();
	if (findComp) { return findComp; }

	// コンポーネントを生成
	IComponent *const component = new T(this);
	std::type_index key = std::type_index(typeid(T));

	// 登録
	componentMap_[key].reset(component);
	// 初期化
	component->Init();

	return GetComponent<T>();
}


template <typename T>
T *const GameObject::GetComponent() const {
	static_assert(std::is_base_of<IComponent, T>::value, "テンプレート型はIComponentクラスの派生クラスではありません");

	const auto &it = componentMap_.find(std::type_index(typeid(T)));
	if (it != componentMap_.end()) {
		return static_cast<T *>(it->second.get());
	}
	return nullptr;
}