#pragma once
#include "Math/Vector3.h"


namespace Lamb {
	namespace Collision {
		/// <summary>
		/// カプセルと円の当たり判定
		/// </summary>
		/// <param name="startPos">カプセルの始まり</param>
		/// <param name="endPos">カプセルの終わり</param>
		/// <param name="capsuleRadius">カプセルの半径</param>
		/// <param name="circlePos">円のポジション</param>
		/// <param name="circleRadius">円の半径</param>
		/// <returns>当たったらtrue</returns>
		bool Capsule(
			const Vector3& startPos, 
			const Vector3& endPos, 
			float capsuleRadius,
			const Vector3& circlePos,
			float circleRadius
		);
	}
}