#include "LocalBodyComp.h"

Vector2 LocalBodyComp::GetGlobalPos() const
{
	return Map::GetGrobalPos(localPos_);
}

void LocalBodyComp::TransfarData()
{
	// グローバル座標に変換する
	transform_.translate = GetGlobalPos();
	// サイズを半分にする
	transform_.scale = size_ * 0.5f;

}
