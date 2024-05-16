#pragma once
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Mat4x4.h" 
#include "Utils/Flg/Flg.h"
#include <vector>
#include "Utils/SafePtr/SafePtr.h"
#include "Drawers/Texture2D/Texture2D.h"

// xが真ん中まで来たらx軸の移動を止める
// yは目的のとこまで移動して目的のとこまで行ったら殺す


class FlaskParticle {
public:
	struct Emitter {
		Vector3 translate;
		Vector3 scale;

		Vector2 radius;
		Vector2 angle;
	};

	struct ParticleStatus {
		Vector3 startPos;
		Vector3 translate;
		Vector3 scale;

		float activeTime = 0.0f;
		float deathTime = 0.0f;

		bool isActive;
	};


public:
	FlaskParticle();
	FlaskParticle(const FlaskParticle&) = default;
	FlaskParticle(FlaskParticle&&) = default;
	~FlaskParticle() = default;

public:
	void Update();
	void Draw(const Mat4x4& camera);

public:
	void Start();

	void Restart();

	void Stop();

	void Pause();

	void Reset();

public:
	void SetEndTranslate(const Vector3& endTranslate) {
		endTranslate_ = endTranslate;
	}

	void Resize(size_t size) {
		particles_.resize(size);
	}

	void SetEmitter(const Emitter& emitter) {
		emitter_ = emitter;
	}

	[[nodiscard]] const Lamb::Flg& GetIsActive() const {
		return isActive_;
	}

	void SetTextureID(uint32_t textureID) {
		textureID_ = textureID;
	}

	void SetDeathTime(Vector2 deathTime) {
		deathTime_ = deathTime;
	}

	void SetRadius(Vector2 radius) {
		emitter_.radius = radius;
	}

	void SetRotate(Vector2 angle) {
		emitter_.angle = angle;
	}

	void SetEmitterPos(const Vector3& pos) {
		emitter_.translate = pos;
	}

	void SetParticleSize(const Vector3& min, const Vector3& max) {
		scaleMin_ = min;
		scaleMax_ = max;
	}

private:
	Vector3 GetRandomVector();

private:
	Emitter emitter_;

	Vector2 freq_;
	float latesetFreq_;
	float randomFreq_;

	uint32_t appParticleNumMin_;
	uint32_t appParticleNumMax_;

	Vector3 scaleMin_;
	Vector3 scaleMax_;

	Vector2 deathTime_;

	std::vector<ParticleStatus> particles_;
	uint32_t curentParticleIndex_;


	Vector3 endTranslate_;

	Lamb::Flg isActive_;

	Lamb::SafePtr<Texture2D> texture2D_;
	uint32_t textureID_;
};