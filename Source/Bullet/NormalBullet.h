#pragma once 

//(弾)スライムクラス

//親クラス
#include "Bullet.h"

//Directionの定数を使いたいので#include
#include "GameDef.h"

class NormalBullet : public Bullet
{
public:
	//初期化
	void Init();

private:

	//衝突時処理
	void OnHit(const Collider& pCollisionA, const Collider& pCollisionB);
};