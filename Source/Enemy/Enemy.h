#pragma once 

//(敵)基底クラス

//音源クラスを#include
#include "Fwk/Audio/SoundSource.h"
//Waveファイルを扱うクラスを#include
#include "Fwk/Audio/WaveSound.h"

#include "Object/Object.h"

class Enemy : public Object
{

public:
	//初期化
	virtual void Init();
	//終了
	virtual void Term();

	//生成された時に呼び出す
	//派生クラスで上書きして使う
	virtual void OnCreated();
protected:

	//ダメージを受ける
	virtual void OnDamaged(int damage);

	//派生ごとの更新
	virtual void PersonalUpdate();
	//速度の更新
	void UpdateVelocity();

	//やられたときに呼び出される
	//派生クラスで上書きして使う
	virtual void OnDefeated();

protected:
	//効果音
	WaveSound mSound;
	//音源
	SoundSource mSoundSource;

	//移動量
	Vector3f mVelocity;
	//方向
	Vector3f mDirection;
	//速度
	float mSpeed;
	//最大速度
	float mMaxSpeed;

	//体力
	int mHP;

	//ダメージ時に加算する赤色の強さ（0.0~1.0）
	float mDamagedColor;
};