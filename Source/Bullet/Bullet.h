#pragma once 

//(弾)基底クラス

//SpriteやTexture,Animationを使うためのヘッダファイル
#include "Fwk/Graphics.h"

//Vector2fクラスを使うためのヘッダファイル
#include "Lib/Math/Vector.h"
using namespace Lib::Math;

//コリジョンクラスを使うのでinclude
#include "Fwk/Collision/Collider.h"
//コリジョンクラスの名前空間を使う
using namespace Fwk::Collision;

//音源クラスを#include
#include "Fwk/Audio/SoundSource.h"
//Waveファイルを扱うクラスを#include
#include "Fwk/Audio/WaveSound.h"

#include "Object/Object.h"

class Bullet : public Object
{

public:
	//初期化
	virtual void Init();
	//終了
	virtual void Term();
	//更新
	virtual void Update();
	
	//向きの設定
	void SetDirection(const Vector3f vDirection);
	//速度の設定
	void SetSpeed(const float speed,const float maxSpeed);
	//大きさの設定
	void SetSize(Sizef size);
	//衝突グループの設定
	void SetCollisionGroup(int collisionGroup);

	//生成された時に呼び出す
	//派生クラスで上書きして使う
	virtual void OnCreated();

private:

	//速度の更新
	void UpdateVelocity();

protected:

	//固有の更新
	virtual void PersonalUpdate();

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

	//生成元のコリジョングループ
	int mCreator;
};