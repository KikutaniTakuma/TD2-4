#include "FlgManager.h"
#include "Utils/Flg/Flg.h"

void FlgManager::AllFlgUpdate() const {
	for (auto const i : flags_) {
		i->Update();
	}
}

FlgManager* const FlgManager::GetInstance() {
	static FlgManager instance;

	return &instance;
}

void FlgManager::Set(Lamb::Flg* const flg) {
	flags_.insert(flg);
}

void FlgManager::Release(Lamb::Flg* const flg) {
	flags_.erase(flg);
}