#pragma once
#include "Utils/Cocepts/Cocepts.h"

namespace Lamb {
	template<IsPtr T>
	void SafeDelete(T& ptr) {
		delete ptr;
		ptr = nullptr;
	}
}