#pragma once
#include "Math/Vector2.h"

class FlaskParticle {
private:
	struct Emitter {
		Vector2 translate;
		Vector2 scale;

	};

	struct ParticleStatus
	{
		Vector2 translate;
		Vector2 scale;
	};

public:
	// xが真ん中まで来たらx軸の移動を止める
	// yは目的のとこまで移動して目的のとこまで行ったら殺す

};