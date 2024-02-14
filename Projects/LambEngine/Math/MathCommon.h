#pragma once
#include <numbers>
#include <limits>

namespace Lamb {
	namespace Math {
		template<std::floating_point Floating>
		constexpr Floating toRadian = static_cast<Floating>(std::numbers::pi_v<double> / 180.0);

		template<std::floating_point Floating>
		constexpr Floating toDegree = static_cast < Floating>(180.0 / std::numbers::pi_v<double>);


	}
}