#pragma once
#include "Engine/Graphics/TextureManager/TextureManager.h"
#include "Engine/Graphics/PipelineManager/PipelineManager.h"
#include "Engine/Buffer/StructuredBuffer/StructuredBuffer.h"

#include "Math/Mat4x4.h"

#include "Utils/Flg/Flg.h"
#include "Utils/Easing/Easing.h"

#include <array>
#include <variant>

/// <summary>
/// パーティクルの描画
/// </summary>
class Particle {
public:
	using Item = std::variant<uint32_t, float, Vector2, Vector3, std::string>;
	using Group = std::unordered_map<std::string, Item>;

public:
	struct MatrixData {
		Mat4x4 wvpMat;
	};

	struct VertexData {
		Vector3 position;
		Vector2 uv;
	};

private:
	struct WorldTransForm {
		Vector2 scale_;
		Vector2 scaleStart_;
		Vector2 scaleSecond_;

		Vector3 rotate_;
		Vector3 rotateStart_;
		Vector3 rotateSecond_;
		Vector3 pos_;
		uint32_t color_;
		
		Vector3 movePos_;
		Vector3 movePosSecond_;
		bool isActive_;

		// スタートした時間
		std::chrono::steady_clock::time_point startTime_;

		// 消える時間
		std::chrono::milliseconds deathTime_;
	};

	enum class EmitterType {
		Cube,
		Circle,
	};

	struct Emitter {
		// エミッターの場所
		Vector3 pos_;

		// エミッターの大きさ
		Vector3 size_;

		// エミッターのタイプ(立方体か球か)
		EmitterType type_;

		// 球のときの半径
		float circleSize_;

		// 球のときのランダム範囲
		std::pair<Vector3, Vector3> rotate_;

		uint32_t particleMaxNum_;

		// 有効時間
		std::chrono::milliseconds validTime_;
	};

	struct Setting {
		Emitter emitter_;

		///
		/// 乱数の範囲
		/// 

		// 大きさ
		std::pair<Vector2, Vector2> size_;
		std::pair<Vector2, Vector2> sizeSecond_;

		// 大きさラープ
		int32_t sizeEaseType_;
		std::function<float(float)> sizeEase_;

		// 移動(速度)
		std::pair<Vector3, Vector3> velocity_;
		std::pair<Vector3, Vector3> velocitySecond_;

		// 移動ラープのタイプ
		int32_t moveEaseType;
		std::function<float(float)> moveEase_;

		// 移動方向
		std::pair<Vector3, Vector3> moveRotate_;

		// 回転
		std::pair<Vector3, Vector3> rotate_;
		std::pair<Vector3, Vector3> rotateSecond_;
		// 大きさラープ
		int32_t rotateEaseType_;
		std::function<float(float)> rotateEase_;



		// 一度にいくつ出すか(数)
		std::pair<uint32_t, uint32_t> particleNum_;

		// 出す頻度(milliseconds)
		std::pair<uint32_t, uint32_t> freq_;

		// 消える時間(milliseconds)
		std::pair<uint32_t, uint32_t> death_;

		// 色
		std::pair<uint32_t, uint32_t> color_;
		int32_t colorEaseType_;
		std::function<float(float)> colorEase_;

		///
		/// 
		/// 
		
		// スタートした時間
		std::chrono::steady_clock::time_point startTime_;

		// 前に出した時間
		std::chrono::steady_clock::time_point durationTime_;

		// 今有効か
		Lamb::Flg isValid_;


		// 縦横の大きさを同じにするか
		Lamb::Flg isSameHW_;
	};

public:
	Particle();
	Particle(uint32_t indexNum);
	Particle(const Particle&);
	Particle(Particle&&) noexcept;
	~Particle();

public:
	Particle& operator=(const Particle& right);
	Particle& operator=(Particle&& right) noexcept;

	inline WorldTransForm& operator[](size_t index) {
		return wtfs_[index];
	}

	inline const WorldTransForm& operator[](size_t index) const {
		return wtfs_[index];
	}


	/// <summary>
	/// 静的メンバ関数
	/// </summary>
public:
	static void Initialize(
		const std::string& vsFileName = "./Resources/Shaders/ParticleShader/Particle.VS.hlsl",
		const std::string& psFileName = "./Resources/Shaders/ParticleShader/Particle.PS.hlsl"
	);

	static void Finalize();

private:
	static void LoadShader(const std::string& vsFileName, const std::string& psFileName);

	static void CreateGraphicsPipeline();

	/// <summary>
	/// 静的メンバ変数
	/// </summary>
private:
	static std::array<Pipeline*, size_t(Pipeline::Blend::BlendTypeNum)> graphicsPipelineState_;
	static Shader shader_;

	static D3D12_INDEX_BUFFER_VIEW indexView_;
	static Lamb::LambPtr<ID3D12Resource> indexResource_;

public:
	void LoadSettingDirectory(const std::string& directoryName);

	void SaveSettingFile(const std::string& groupName);
private:
	void LopadSettingFile(const std::string& jsonName);

	void BackUpSettingFile(const std::string& groupName);
private:
	std::unordered_map<std::string, Group> datas_;
	std::string dataDirectoryName_;


public:
	void LoadTexture(const std::string& fileName);
	void ThreadLoadTexture(const std::string& fileName);

public:
	/// <summary>
	/// パーティクルスタート関数
	/// </summary>
	void ParticleStart();

	/// <summary>
	/// パーティクルスタート関数
	/// </summary>
	/// <param name="emitterPos">Emitterの位置</param>
	void ParticleStart(const Vector3& pos);

	/// <summary>
	/// パーティクル停止関数
	/// </summary>
	void ParticleStop();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画関数
	/// </summary>
	/// <param name="viewProjection">カメラの行列</param>
	/// <param name="blend">ブレンドモード</param>
	void Draw(
		const Vector3& cameraRotate,
		const Mat4x4& viewProjection,
		Pipeline::Blend blend = Pipeline::Blend::Normal
	);

	/// <summary>
	/// この関数で設定及び保存等を行う
	/// </summary>
	/// <param name="guiName">ImGuiの名前</param>
	void Debug(const std::string& guiName);

	/// <summary>
	/// テクスチャのサイズ取得
	/// </summary>
	/// <returns></returns>
	Vector2 GetTexSize() const {
		if (tex_) {
			return tex_->getSize();
		}
		else {
			return Vector2::kZero;
		}
	}

	/// <summary>
	/// アニメーションスタート関数
	/// </summary>
	/// <param name="aniUvPibot">アニメーションをスタートさせる場所</param>
	void AnimationStart(float aniUvPibot = 0.0f);

	/// <summary>
	/// アニメーション関数
	/// </summary>
	/// <param name="aniSpd">アニメーション速度(milliseconds)</param>
	/// <param name="isLoop">アニメーションをループさせるか</param>
	/// <param name="aniUvStart">アニメーションをスタートさせる場所</param>
	/// <param name="aniUvEnd">アニメーションを終わらせる場所</param>
	void Animation(size_t aniSpd, bool isLoop, float aniUvStart, float aniUvEnd);

	/// <summary>
	/// アニメーションを一時停止
	/// </summary>
	void AnimationPause();

	/// <summary>
	/// アニメーションを再スタート
	/// </summary>
	void AnimationRestart();

	/// <summary>
	/// アニメーションしているかを取得
	/// </summary>
	/// <returns>アニメーションフラグ</returns>
	bool GetIsAnimation()const {
		return isAnimation_;
	}

	/// <summary>
	/// パーティクルの量を返る
	/// </summary>
	/// <param name="index">particleのインデックス</param>
	void Resize(uint32_t index);

	const Lamb::Flg& GetIsParticleStart() const {
		if (settings_.empty()) {
			static Lamb::Flg result{false};
			return result;
		}

		return settings_.front().isValid_;
	}

	bool GetIsClose() const {
		return isClose_;
	}

public:
	Vector2 uvPibot;
	Vector2 uvSize;

	Vector3 emitterPos;

private:
	std::deque<Setting> settings_;

	// ループするか
	Lamb::Flg isLoop_;

	uint32_t currentSettingIndex_;
	uint32_t currentParticleIndex_;

	std::vector<WorldTransForm> wtfs_;

	class CbvSrvUavHeap* srvHeap_;


	D3D12_VERTEX_BUFFER_VIEW vertexView_;
	Lamb::LambPtr<ID3D12Resource> vertexResource_;

	StructuredBuffer<Mat4x4> wvpMat_;
	StructuredBuffer<Vector4> colorBuf_;

	Texture* tex_;
	bool isLoad_;

	bool isBillboard_;
	bool isYBillboard_;

	// アニメーション変数
	std::chrono::steady_clock::time_point aniStartTime_;
	float aniCount_;
	bool isAnimation_;

	bool isClose_;
public:
	float uvPibotSpd_;
};