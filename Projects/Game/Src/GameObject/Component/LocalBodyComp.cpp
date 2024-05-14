#include "LocalBodyComp.h"

Vector2 LocalBodyComp::GetGlobalPos() const
{
	return BlockMap::GetGlobalPos(localPos_);
}

void LocalBodyComp::TransfarData()
{
	// グローバル座標に変換する
	transform_.translate = Vector3{ GetGlobalPos(),-1.f };
	// サイズを半分にする
	transform_.scale = size_ * drawScale_;

}
