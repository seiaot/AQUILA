#pragma once 

//(弾)スライムクラス

//親クラス
#include "Effect.h"

//Directionの定数を使いたいので#include
#include "GameDef.h"

class ExplosionEX : public Effect
{
public:
	//初期化
	void Init();

	//アニメーションの初期化
	void InitAnimation();

	//アニメーションのコールバック
	void OnAnimationEvent(const AnimationEvent& animEvent);

private:

	//衝突時処理
	void OnHit(const Collider& pCollisionA, const Collider& pCollisionB);
};