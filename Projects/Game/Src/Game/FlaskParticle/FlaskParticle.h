#pragma once
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

		float maxRotate_;
		float minRotate_;
	};

	struct ParticleStatus {
		Vector3 translate;
		Vector3 scale;

		float activeTime = 0.0f;

		bool isActive;
	};

#ifndef NORETURN
#define NORETURN [[noreturn]] void
#endif // !NORETURN


public:
	FlaskParticle();
	FlaskParticle(const FlaskParticle&) = default;
	FlaskParticle(FlaskParticle&&) = default;
	~FlaskParticle() = default;

public:
	NORETURN Update();
	NORETURN Draw(const Mat4x4& camera);

public:
	NORETURN Start();

	NORETURN Restart();

	NORETURN Stop();

	NORETURN Pause();

public:
	NORETURN SetEndTranslate(const Vector3& endTranslate) {
		endTranslate_ = endTranslate;
	}

	NORETURN Resize(size_t size) {
		particles_.resize(size);
	}

	NORETURN SetEmitter(const Emitter& emitter) {
		emitter_ = emitter;
	}

	[[nodiscard]] const Lamb::Flg& GetIsActive() const {
		return isActive_;
	}

private:
	Emitter emitter_;

	float freqMin;
	float freqMax;

	uint32_t appParticleNumMin_;
	uint32_t appParticleNumMax_;

	Vector3 scaleMin;
	Vector3 scaleMax;

	float deathTime_;

	std::vector<ParticleStatus> particles_;
	uint32_t curentParticleIndex_;


	Vector3 endTranslate_;

	Lamb::Flg isActive_;

	Lamb::SafePtr<Texture2D> texture2D_;
	uint32_t textureID_;

private:
#ifdef NORETURN
#undef NORETURN
#endif // NORETURN
};