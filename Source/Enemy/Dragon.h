#pragma once 

//(敵)スライムクラス

//親クラス
#include "Enemy.h"

#include "DragonBody.h"

class Dragon : public Enemy
{
	//敵の状態
	enum class Status
	{
		Idle,
		Rushing,
		Fire,
		Return,
	};

public:
	//初期化
	void Init();
	//終了
	void Term();

	//位置の設定
	void SetPosition(Vector3f vPos);

	//派生ごとの更新
	virtual void PersonalUpdate();

	bool Isinactive();

	//生成された時に呼び出す
	void OnCreated();

private:

	//やられたとき
	void OnDefeated();

	void OnHit(const Collider& pCollisionA, const Collider& pCollisionB);

private:
	//体
	DragonBody* pBody[30];

	//移動の起点となる位置
	Vector3f mBasePos;
	//少し前にいた位置
	Vector3f mBefPos;
	//移動先の座標
	Vector3f mTargetPos;

	//裏面
	Texture mInverse;

	//状態
	Status mStatus;

	//XYの移動量
	float mMoveX;
	float mMoveY;
	//移動の生成に使用
	float mRotX;
	float mRotY;
	//毎フレーム角度に加算を行う値
	float mAddRotX;
	float mAddRotY;
	//追加する角度
	float mMoveWaveX;
	float mMoveWaveY;

	//タイマー
	float mTimer;
	float mSavePosTimer;

	//炎を放ったかどうか
	bool mToggleFire;
};