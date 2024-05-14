#include "FlaskParticle.h"
#include "Drawers/DrawerManager.h"
#include "Utils/EngineInfo/EngineInfo.h"
#include "Utils/Random/Random.h"

FlaskParticle::FlaskParticle():
    emitter_(),
    freqMin(0.1f),
    freqMax(0.2f),
    appParticleNumMin_(5),
    appParticleNumMax_(10),
    particles_(100),
    curentParticleIndex_(0u),
    isActive_()
{
    texture2D_ = DrawerManager::GetInstance()->GetTexture2D();
    textureID_ = DrawerManager::GetInstance()->LoadTexture("./Resources/Ball.png");
    scaleMin = Vector3::kIdentity * 20.0f;
    scaleMax = Vector3::kIdentity * 30.0f;
}

void FlaskParticle::Update() {
    if (isActive_) {
        float deltatime = Lamb::DeltaTime();

        uint32_t appParticle = Lamb::Random(appParticleNumMin_, appParticleNumMax_);

        for (size_t index = curentParticleIndex_; index < (curentParticleIndex_ + appParticle) and index < particles_.size(); index++) {
            particles_[index].isActive = true;
            particles_[index].activeTime = 0.0f;
            particles_[index].scale = Lamb::Random(scaleMin, scaleMax);
        }

        curentParticleIndex_ += appParticle;


    }
}

void FlaskParticle::Draw(const Mat4x4& camera) {
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
    
}

void FlaskParticle::Restart() {
    
}

void FlaskParticle::Stop() {
    
}

void FlaskParticle::Pause() {
    
}
