#include "MathCommon.h"
#include <cmath>

namespace Lamb {
    namespace Math {
        float LengthSQ(const std::initializer_list<float>& data)
        {
            float result = 0.0f;
            for (const auto& i : data) {
                result += i * i;
            }
            return result;
        }

        float Length(const std::initializer_list<float>& data)
        {
            return std::sqrt(LengthSQ(data));
        }
    }
}