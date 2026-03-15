#pragma once 

//(弾)スライムクラス

//親クラス
#include "Effect.h"

//Directionの定数を使いたいので#include
#include "GameDef.h"

class SubExplosion : public Effect
{
public:
	//初期化
	void Init();

	//アニメーションの初期化
	void InitAnimation();

private:

	//衝突時処理
	void OnHit(const Collider& pCollisionA, const Collider& pCollisionB);
};