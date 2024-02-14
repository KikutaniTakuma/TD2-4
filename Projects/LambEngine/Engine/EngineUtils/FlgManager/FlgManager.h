#pragma once
#include <unordered_set>

namespace Lamb {
	class Flg;
}

class FlgManager final {
	friend class Lamb::Flg;

private:
	FlgManager() = default;
	FlgManager(const FlgManager&) = delete;
	FlgManager(FlgManager&&) = delete;
	~FlgManager() = default;

	FlgManager& operator=(const FlgManager&) = delete;
	FlgManager& operator=(FlgManager&&) = delete;


public:
	static FlgManager* const GetInstance();

public:
	void AllFlgUpdate() const;

private:
	void Set(Lamb::Flg* const flg);

	void Release(Lamb::Flg* const flg);

private:
	std::unordered_set<Lamb::Flg*> flags_;
};