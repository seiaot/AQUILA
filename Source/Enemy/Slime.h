#pragma once 

//(敵)スライムクラス

//親クラス
#include "Enemy.h"

//Directionの定数を使いたいので#include
#include "GameDef.h"

class Slime : public Enemy
{
	//敵の状態
	enum class Status
	{
		Moving,		//移動中
	};

public:
	//初期化
	void Init();

	//派生ごとの更新
	virtual void PersonalUpdate();

bool Isinactive();

private:

	void OnHit(const Collider& pCollisionA, const Collider& pCollisionB);

private:

	//タイマー
	float mTimer;

	//射撃したか
	bool mFired;
};