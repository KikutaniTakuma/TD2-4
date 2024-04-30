#include "LocalBodyComp.h"

Vector2 LocalBodyComp::LocalToGlobal() const
{
	return Map::GetGrobalPos(localPos_);
}