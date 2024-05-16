#include "FlaskParticle.h"
#include "Drawers/DrawerManager.h"
#include "Utils/EngineInfo/EngineInfo.h"
#include "Utils/Random/Random.h"
#include "Utils/Easeing/Easeing.h"
#include <numbers>

FlaskParticle::FlaskParticle():
    emitter_(),
    freq_(0.1f, 0.2f),
    latesetFreq_(0.0f),
    randomFreq_(0.0f),
    appParticleNumMin_(5),
    appParticleNumMax_(10),
    particles_(100),
    curentParticleIndex_(0u),
    isActive_()
{
    texture2D_ = DrawerManager::GetInstance()->GetTexture2D();
    textureID_ = DrawerManager::GetInstance()->LoadTexture("./Resources/Ball.png");
    scaleMin_ = Vector3::kIdentity * 2.0f;
    scaleMax_ = Vector3::kIdentity * 3.0f;

    emitter_.radius = { 10.0f, 20.0f };
    emitter_.angle = { std::numbers::pi_v<float> / 3.0f, 2.0f * std::numbers::pi_v<float> / 3.0f };

    deathTime_ = { 0.3f, 0.8f };

    randomFreq_ = Lamb::Random(freq_.min, freq_.max);
}

void FlaskParticle::Update() {
    if (isActive_) {
        if (randomFreq_ < latesetFreq_ and curentParticleIndex_ < static_cast<uint32_t>(particles_.size())) {
            uint32_t appParticle = Lamb::Random(appParticleNumMin_, appParticleNumMax_);

            for (size_t index = curentParticleIndex_; index < (curentParticleIndex_ + appParticle) and index < particles_.size(); index++) {
                particles_[index].isActive = true;
                particles_[index].activeTime = 0.0f;
                particles_[index].startScale = Lamb::Random(scaleMin_, scaleMax_);
                particles_[index].startPos = GetRandomVector();
                particles_[index].deathTime = Lamb::Random(deathTime_.min, deathTime_.max);
            }

            curentParticleIndex_ += appParticle;
            
            latesetFreq_ = 0.0f;
            randomFreq_ = Lamb::Random(freq_.min, freq_.max);
        }

        float deltatime = Lamb::DeltaTime();
        latesetFreq_ += deltatime;
        for (auto i = particles_.begin(); i != particles_.end(); i++) {
            if (i->isActive) {
                float t = i->activeTime / i->deathTime;
                i->translate = Vector3::Lerp(i->startPos, endTranslate_, Easeing::OutExpo(t));
                i->scale= Vector3::Lerp(i->startScale, i->startScale * 0.1f, t);
                if (1.0f <= t) {
                    i->isActive = false;
                    if (i == (--particles_.end())) {
                        isActive_ = false;
                    }
                }
                i->activeTime += deltatime;
            }
        }
    }
}

void FlaskParticle::Draw(const Mat4x4& camera) {
    if (not isActive_) {
        return;
    }
    for (auto& particle : particles_) {
        if (particle.isActive) {
            texture2D_->Draw(
                Mat4x4::MakeAffin(particle.scale, Vector3::kZero, particle.translate),
                Mat4x4::kIdentity,
                camera,
                textureID_,
                0xffffffff,
                BlendType::kNormal
            );
        }
    }
}

void FlaskParticle::Start() {
    Reset();
    Restart();
}

void FlaskParticle::Restart() {
    isActive_ = true;
}

void FlaskParticle::Stop() {
    Reset();
    Pause();
}

void FlaskParticle::Pause() {
    isActive_ = false;
}

void FlaskParticle::Reset() {
    curentParticleIndex_ = 0;
    latesetFreq_ = 0.0f;
    randomFreq_ = Lamb::Random(freq_.min, freq_.max);
}

Vector3 FlaskParticle::GetRandomVector() {
    Vector3 result;

    float angle = Lamb::Random(emitter_.angle.min, emitter_.angle.max);
    float radius = Lamb::Random(emitter_.radius.min, emitter_.radius.max);

    result *= Mat4x4::MakeTranslate(Vector3::kXIdentity * radius) * Mat4x4::MakeRotateZ(angle) * Mat4x4::MakeTranslate(emitter_.translate);

    return result;
}
