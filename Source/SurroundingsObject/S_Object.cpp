#include "S_Object.h"

//コリジョングループの列挙型を使うために#include
#include "GameDef.h"
//CollisionManagerを使うのでinclude
#include "Fwk/Framework.h"

//初期化
void S_Object::Init()
{
	Object::Init();

	//Waveデータ読み込み
	mSound.Load("Sound/DamageE.wav");

	//コリジョンの設定(共通項目のみ)
	{
		mCollision.SetOutlineColor(1.0f, 1.0f, 0.0f);
	}

}

//後片付け
void S_Object::Term()
{
	Object::Term();

	//サウンドの破棄
	mSound.Unload();
	//音源の解放
	mSoundSource.Term();
}

//更新
void S_Object::Update()
{
	Object::Update();
}

//固有の更新
void S_Object::PersonalUpdate()
{
	//移動量の更新
	UpdateVelocity();
	//位置に移動速度を足す
	mPos += mVelocity;
}

//速度の更新
void S_Object::UpdateVelocity()
{
	float acceleration = mSpeed * Time_I->GetDeltaTime();	//このフレーム速度
	float maxSpeed = mMaxSpeed * Time_I->GetDeltaTime();	//最大速度

	//向きに合わせて加速
	mVelocity += mDirection * acceleration;

	//速度が上限を超えないようにする
	if (mVelocity.GetLength() > maxSpeed)
	{
		mVelocity = mVelocity.GetUnitVector() * maxSpeed;
	}
}

//向きの設定
void S_Object::SetDirection(const Vector3f vDirection)
{
	mDirection = vDirection;
}

//速度の設定
void S_Object::SetSpeed(const float speed,const float maxSpeed)
{
	mSpeed = speed;
	mMaxSpeed = maxSpeed;
}

//大きさの設定
void S_Object::SetSize(Sizef size)
{
	mSize = size;
	mCollision.SetCubeSize(size);
}

//衝突グループの設定
void S_Object::SetCollisionGroup(int collisionGroup)
{
	mCollision.ClearGroup();
	mCollision.ClearHitGroup();

	//作られたとき
	if (collisionGroup == (int)CollisionGroup::S_Object)
	{
		mCollision.SetGroup((int)CollisionGroup::S_Object);
		mCollision.AddHitGroup((int)CollisionGroup::Player);
		mCollision.AddHitGroup((int)CollisionGroup::Enemy);
	}
}

//生成された時に呼び出す
//派生クラスで上書きして使う
void S_Object::OnCreated()
{
	//なし
}