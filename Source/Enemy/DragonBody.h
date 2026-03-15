#pragma once 

//親クラス
#include "Enemy.h"

//Directionの定数を使いたいので#include
#include "GameDef.h"

class Dragon;

class DragonBody : public Enemy
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
	void PersonalUpdate();

	void SetBeforPos();
	Vector3f GetBeforePos() const;

	bool Isinactive();

private:

	void OnHit(const Collider& pCollisionA, const Collider& pCollisionB);

private:

	//少し前の位置
	Vector3f mBefPos;

	//タイマー
	float mTimer;
};