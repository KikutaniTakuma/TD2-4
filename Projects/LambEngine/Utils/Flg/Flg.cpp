#include "Utils/Flg/Flg.h"
#include "Engine/EngineUtils/FlgManager/FlgManager.h"

namespace Lamb {
	Flg::Flg() :
		flg_(false),
		preFlg_(false)
	{
		static FlgManager* const flgManager = FlgManager::GetInstance();
		flgManager->Set(this);
	}

	Flg::Flg(const Flg& right):
		Flg{}
	{
		*this = right;
	}
	Flg::Flg(Flg&& right) noexcept :
		Flg{}
	{
		*this = right;
	}

	Flg::~Flg() {
		static FlgManager* const flgManager = FlgManager::GetInstance();
		flgManager->Release(this);
	}

	Flg::Flg(bool right) :
		flg_(right),
		preFlg_(false)
	{
		static FlgManager* const flgManager = FlgManager::GetInstance();
		flgManager->Set(this);
	}

	void Flg::Update() {
		preFlg_ = flg_;
	}
}