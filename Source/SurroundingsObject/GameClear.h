#pragma once 

//(弾)スライムクラス

//親クラス
#include "S_Object.h"

//Directionの定数を使いたいので#include
#include "GameDef.h"

class GameClear : public S_Object
{
public:
	//初期化
	void Init();

	//派生の更新
	void PersonalUpdate();

private:

	//衝突時処理
	void OnHit(const Collider& pCollisionA, const Collider& pCollisionB);
};